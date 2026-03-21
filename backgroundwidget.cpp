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
#include "Data.h"
BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(std::make_unique<QGraphicsScene>())
    , m_overlay(std::make_unique<Overlay>())
    , m_player_1(std::make_unique<Player>())
    , m_player_2(std::make_unique<Player>())
    , is_player_1(true)
    , ptype(PlayerType::NONPLAYER)
{

    qDebug() << "BackgroundWidget"<<size();
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

    // 3. 创建透明覆盖层（一个空的 QGraphicsWidget）
    m_overlay->setZValue(Layer::LAYER_PLAYER_3);                     // 确保覆盖层在最上面
    m_overlay->setGeometry(QRectF(0, 0, width(), height()));
    m_scene->addItem(m_overlay.get());

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
    setBackground(filePath1,m_player_1.get(),true);
    setBackground(filePath2,m_player_2.get(),false);
    // 设置当前文件名为第一个文件的文件名
    QFileInfo info(filePath1);
    m_currentFile = info.fileName();
}


void BackgroundWidget::setPath(const QString &filePath)
{
    if(is_player_1)
    {
        m_player_2->showPlayer(ptype);
        setBackground(filePath,m_player_1.get(),false);
        is_player_1 = false;
    }
    else
    {
        m_player_1->showPlayer(ptype);
        setBackground(filePath,m_player_2.get(),false);
        is_player_1 = true;
    }
    // 只存储文件名，而不是完整路径
    QFileInfo info(filePath);
    m_currentFile = info.fileName();
}

void BackgroundWidget::setBackground(const QString &filePath1,Player * player,bool isplayer)
{
    QFileInfo info(filePath1);
    QString suffix = info.suffix().toLower();
        
    //判断是否为后台加载
    if(!isplayer)
    {
        player->hidePlayer(PlayerType::NONPLAYER);
    }
    // 根据文件类型设置播放器
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg") {
        if(isplayer){player->showPlayer(PlayerType::PIXMAP);}
        else {ptype = PlayerType::PIXMAP;}
        player->setupPixmap(filePath1);
    } else if (suffix == "gif") {
        if(isplayer){player->showPlayer(PlayerType::MOVIE);}
        else {ptype = PlayerType::MOVIE;}
        player->setupMovie(filePath1);
    } else if (suffix == "mp4") {
        if(isplayer){player->showPlayer(PlayerType::VIDEO);}
        else {ptype = PlayerType::VIDEO;}
        player->setupVideo(filePath1);
    } else {
        return;
    }
    // 调整覆盖层大小以适应新背景
    if (m_overlay)
        m_overlay->setGeometry(QRectF(0, 0, width(), height()));

    // 确保覆盖层在最上层（Z序）
    m_overlay->setZValue(10);
}


const QString &BackgroundWidget::getFile() const
{
    return m_currentFile;
}

QGraphicsWidget* BackgroundWidget::getOvrlay() const
{
    return m_overlay.get();
}

void BackgroundWidget::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (m_scene) {
        // 调整场景大小
        m_scene->setSceneRect(0, 0, width(), height());
        
        // 调整覆盖层大小
        if (m_overlay) {
            m_overlay->setGeometry(QRectF(0, 0, width(), height()));
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
