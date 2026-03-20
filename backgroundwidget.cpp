#include "backgroundwidget.h"
#include "Overlay.h"
#include "Player.h"
#include <QGraphicsVideoItem>
#include <QGraphicsWidget>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QResizeEvent>
#include <QPainter>
#include <QDebug>
#include <QUrl>

BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(std::make_unique<QGraphicsScene>())
    , m_overlay(std::make_unique<Overlay>())
    , m_player_1(std::make_unique<Player>())
    , m_player_2(std::make_unique<Player>())
    , m_currentPlayer(nullptr)
{
    qDebug() << "BackgroundWidget constructor started";
    //设置背景，视图属性，创建覆盖层基本属性
    qDebug() << "Calling setBaseQWidget()";
    setBaseQWidget();
    qDebug() << "setBaseQWidget() completed";
    //设置相关播放器
    qDebug() << "Calling setPlayer()";
    setPlayer();
    qDebug() << "setPlayer() completed";
    qDebug() << "BackgroundWidget constructor completed";
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
    // 初始化两个播放器
    m_player_1->setPlayer();
    m_player_2->setPlayer();
    
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
                            qDebug() << "重新设置视频大小成功";
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

void BackgroundWidget::setBackground(const QString &filePath)
{
    QFileInfo info(filePath);
    QString suffix = info.suffix().toLower();
    
    // 选择当前未使用的播放器
    Player* nextPlayer = (m_currentPlayer == m_player_1.get()) ? m_player_2.get() : m_player_1.get();
    
    // 隐藏下一个播放器的所有类型
    nextPlayer->hidePlayer(PlayerType::NONPLAYER);
    
    // 根据文件类型设置播放器
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg") {
        nextPlayer->switchPlayer(PlayerType::PIXMAP);
        nextPlayer->setupPixmap(filePath);
    } else if (suffix == "gif") {
        nextPlayer->switchPlayer(PlayerType::MOVIE);
        nextPlayer->setupMovie(filePath);
    } else if (suffix == "mp4") {
        nextPlayer->switchPlayer(PlayerType::VIDEO);
        nextPlayer->setupVideo(filePath);
    } else {
        qWarning() << "Unsupported format:" << suffix;
        return;
    }
    
    // 隐藏当前活跃的播放器
    if (m_currentPlayer) {
        m_currentPlayer->hidePlayer(m_currentPlayer->getCurrentPlayerType());
    }
    
    // 更新当前活跃播放器
    m_currentPlayer = nextPlayer;
    
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
