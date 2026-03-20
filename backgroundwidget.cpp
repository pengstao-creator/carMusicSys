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

BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(std::make_unique<QGraphicsScene>())
    , m_overlay(std::make_unique<Overlay>())
    , m_player(std::make_unique<Player>())
{
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
    m_overlay->setZValue(10);                     // 确保覆盖层在最上面
    m_overlay->setGeometry(QRectF(0, 0, width(), height()));
    m_scene->addItem(m_overlay.get());

    // 4. 设置背景色（默认黑色）
    setBackgroundBrush(Qt::black);
}

void BackgroundWidget::setPlayer()
{
    // 初始化播放器
    m_player->setPlayer();
    
    // 将播放器的图形项添加到场景
    m_scene->addItem(m_player->getPixmapItem());
    m_scene->addItem(m_player->getVideoItem());
    
    // 初始时隐藏所有播放器
    m_player->hidePlayer(PlayerType::NONPLAYER);
    
    // 调整视频项大小以填充视图
    connect(m_player->getMediaPlayer(), &QMediaPlayer::mediaStatusChanged, this, 
            [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia) {
                    auto videoItem = m_player->getVideoItem();
                    QSizeF videoSize = videoItem->nativeSize();
                    if (videoSize.isValid()) {
                            // 设置宽高比模式
                            videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
                            // 然后设置目标矩形仍为视图大小
                            videoItem->setSize(size());
                        }
                }
            });
}

BackgroundWidget::~BackgroundWidget()
{
    // 场景和所有图形项会被自动删除（因为场景设置了父对象为 this）
}

void BackgroundWidget::setBackground(const QString &filePath)
{
    QFileInfo info(filePath);
    QString suffix = info.suffix().toLower();
    
    // 根据文件类型设置播放器
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg") {
        m_player->switchPlayer(PlayerType::PIXMAP);
        m_player->setupPixmap(filePath);
    } else if (suffix == "gif") {
        m_player->switchPlayer(PlayerType::MOVIE);
        m_player->setupMovie(filePath);
    } else if (suffix == "mp4") {
        bool switched = m_player->switchPlayer(PlayerType::VIDEO);
        // 即使类型相同，也要确保视频项可见
        if (!switched) {
            m_player->getVideoItem()->setVisible(true);
        }
        m_player->setupVideo(filePath);
        // 确保视频项大小正确
        m_player->setVideoSize(size());
    } else {
        return;
    }
    
    // 确保播放器在正确的Z序位置
    if (suffix == "mp4") {
        m_player->getVideoItem()->setZValue(0);
    } else {
        m_player->getPixmapItem()->setZValue(0);
    }
    
    m_currentFile = info.fileName();

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
        
        // 调整播放器的视频项和图片项大小
        // 调整视频项大小
        auto videoItem = m_player->getVideoItem();
        if (videoItem) {
            // 设置宽高比模式
            videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
            // 然后设置目标矩形仍为视图大小
            videoItem->setSize(size());
        }
        
        // 调整图片项大小
        auto pixmapItem = m_player->getPixmapItem();
        if (pixmapItem) {
            QPixmap pix = pixmapItem->pixmap();
            if (!pix.isNull()) {
                pixmapItem->setScale(qreal(width()) / pix.width());
            }
        }
    }
}
