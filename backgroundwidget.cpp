#include "backgroundwidget.h"
#include "Overlay.h"
#include "Player.h"
#include <QGraphicsVideoItem>
#include <QGraphicsWidget>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QResizeEvent>
#include <QPainter>
#include <QUrl>
#include <QHash>
#include "Data.h"

// 初始化静态成员变量
BackgroundWidget* BackgroundWidget::_this = nullptr;
BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(std::make_unique<QGraphicsScene>())
    , m_player_1(std::make_unique<Player>())
    , m_player_2(std::make_unique<Player>())
    , is_player_1(true)
    , ptype(PlayerType::NONPLAYER)
{
    _this = this;
    //设置背景，视图属性，创建覆盖层基本属性

    setBaseQWidget();

    //设置相关播放器
    setPlayer();
}


void BackgroundWidget::setBaseQWidget()
{
    // 1. 创建场景
    setScene(m_scene.get());

    // 2. 设置视图属性
    setFrameStyle(QFrame::NoFrame);               // 无边框
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing);        // 抗锯齿
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // 避免残影
    setRenderHint(QPainter::SmoothPixmapTransform); // 平滑缩放
    setAlignment(Qt::AlignCenter);

    // 4. 设置背景色（默认黑色）
    setBackgroundBrush(Qt::black);
}

void BackgroundWidget::setPlayer()
{
    // 初始化两个播放器
    m_player_1->setPlayer(Layer::LAYER_PLAYER_2);
    m_player_2->setPlayer(Layer::LAYER_PLAYER_1);

    // 将两个播放器的图形项添加到场景
    m_scene->addItem(m_player_1->getPixmapItem());
    m_scene->addItem(m_player_1->getVideoItem());
    m_scene->addItem(m_player_2->getPixmapItem());
    m_scene->addItem(m_player_2->getVideoItem());

    // 初始时隐藏所有播放器
    m_player_1->hidePlayer(PlayerType::NONPLAYER);
    m_player_2->hidePlayer(PlayerType::NONPLAYER);

    // 调整视频项大小以填充视图
    auto setupVideoItem = [this](Player* player) {
        connect(player->getMediaPlayer(), &QMediaPlayer::mediaStatusChanged, this,
                [this, player](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia) {
                        auto videoItem = player->getVideoItem();
                        QSizeF videoSize = videoItem->nativeSize();
                        if (videoSize.isValid()) {
                            // 设置宽高比模式
                            videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
                            // 然后设置目标矩形仍为视图大小
                            videoItem->setSize(size());
                        }

                    }
                });
    };

    setupVideoItem(m_player_1.get());
    setupVideoItem(m_player_2.get());
}

BackgroundWidget::~BackgroundWidget()
{
    // 场景和所有图形项会被自动删除（因为场景设置了父对象为 this）
}
void BackgroundWidget::setPathFirst(const QString &filePath1, const QString &filePath2)
{
    setBackground(filePath1,m_player_1.get());
    m_player_1->showPlayer(ptype);
    setBackground(filePath2,m_player_2.get());
    m_player_2->hidePlayer(PlayerType::NONPLAYER);
    // 设置当前文件名为第二个文件的文件名
    QFileInfo info(filePath2);
    m_currentFile = info.fileName();
    qDebug() << "m_player_1"<<filePath1;
}


void BackgroundWidget::setPath(const QString &filePath)
{
    if(is_player_1)
    {
        m_player_2->showPlayer(ptype);
        qDebug() << "m_player_2"<<filePath;

        setBackground(filePath,m_player_1.get());
        m_player_1->hidePlayer(PlayerType::NONPLAYER);
        is_player_1 = false;
    }
    else
    {
        m_player_1->showPlayer(ptype);
        qDebug() << "m_player_1"<<filePath;

        setBackground(filePath,m_player_2.get());
        m_player_2->hidePlayer(PlayerType::NONPLAYER);
        is_player_1 = true;
    }
    // 只存储文件名，而不是完整路径
    QFileInfo info(filePath);
    m_currentFile = info.fileName();

}

void BackgroundWidget::setBackground(const QString &filePath1,Player * player)
{
    QFileInfo info(filePath1);
    QString suffix = info.suffix().toLower();

    // 根据文件类型设置播放器
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg") {
        player->setupPixmap(filePath1);
        ptype = PlayerType::PIXMAP;
    } else if (suffix == "gif") {
        player->setupMovie(filePath1);
        ptype = PlayerType::MOVIE;
    } else if (suffix == "mp4") {
        player->setupVideo(filePath1);
        ptype = PlayerType::VIDEO;
    } else {
        return;
    }
    //调整覆盖层大小
    if (!m_overlay.isEmpty()) {
        for(auto overlay : m_overlay)
        {
            overlay->setGeometry(QRectF(0, 0, width(), height()));
            overlay->setZValue(Layer::LAYER_PLAYER_3);
        }
    }
}


const QString &BackgroundWidget::getFile() const
{
    return m_currentFile;
}

const QHash<QString,Overlay*>& BackgroundWidget::getOvrlay()
{
    return _this->m_overlay;
}

QGraphicsScene *BackgroundWidget::getScene() const
{
    return m_scene.get();
}

void BackgroundWidget::addOvrlay(const std::pair<QString, Overlay *> &overlay)
{
    auto _overlay = overlay.second;
    double LAYER_PALYER = Layer::LAYER_PLAYER_3 + m_overlay.size();
    // 3. 创建透明覆盖层（一个空的 QGraphicsWidget）
    _overlay->setZValue(LAYER_PALYER);                     // 确保覆盖层在最上面
    _overlay->setGeometry(QRectF(0, 0,width(), height()));
    m_scene->addItem(_overlay);
    m_overlay.insert(overlay.first, overlay.second);
}

void BackgroundWidget::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    qDebug() << "resizeEvent" <<size();
    if (m_scene) {
        // 调整场景大小
        m_scene->setSceneRect(0, 0, width(), height());

        //调整覆盖层大小
        if (!m_overlay.isEmpty()) {
            for(auto overlay : m_overlay)
            {
                overlay->setGeometry(QRectF(0, 0, width(), height()));
            }
        }

        // 调整两个播放器的视频项和图片项大小
        auto adjustPlayer = [this](Player* player) {
            if (!player) return;

            // 调整视频项大小
            auto videoItem = player->getVideoItem();
            if (videoItem) {
                // 设置宽高比模式
                videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
                // 然后设置目标矩形仍为视图大小
                videoItem->setSize(size());
            }

            // 调整图片项大小
            auto pixmapItem = player->getPixmapItem();
            if (pixmapItem) {
                QPixmap pix = pixmapItem->pixmap();
                if (!pix.isNull()) {
                    pixmapItem->setScale(qreal(width()) / pix.width());
                }
            }
        };

        adjustPlayer(m_player_1.get());
        adjustPlayer(m_player_2.get());
    }
}
