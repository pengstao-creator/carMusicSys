#include "wallpaerWidget.h"
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
#include "zaxiscontrol.h"
// 初始化静态成员变量
wallpaerWidget::wallpaerWidget(zAxisControl *zAxisCtrl,QObject *parent)
    : QObject(parent)
    , zAxis_Ctrl(zAxisCtrl)
    , m_player_1(std::make_unique<Player>())
    , m_player_2(std::make_unique<Player>())
    , is_player_1(true)
    , ptype(PlayerType::NONPLAYER)
{
    //设置相关播放器
    connect(zAxisCtrl,&zAxisControl::resized,this,&wallpaerWidget::resizeEvent);
    setPlayer();
}

void wallpaerWidget::setPlayer()
{
    // 初始化两个播放器
    m_player_1->setWallpaperPlayer(Layer::LAYER_PLAYER_2);
    m_player_2->setWallpaperPlayer(Layer::LAYER_PLAYER_1);

    // 将两个播放器的图形项添加到场景
    zAxis_Ctrl->getScene()->addItem(m_player_1->getPixmapItem());
    zAxis_Ctrl->getScene()->addItem(m_player_1->getVideoItem());
    zAxis_Ctrl->getScene()->addItem(m_player_2->getPixmapItem());
    zAxis_Ctrl->getScene()->addItem(m_player_2->getVideoItem());

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
                            videoItem->setSize(zAxis_Ctrl->getQRect().size());
                        }

                    }
                });
    };

    setupVideoItem(m_player_1.get());
    setupVideoItem(m_player_2.get());
}

wallpaerWidget::~wallpaerWidget()
{
    // 场景和所有图形项会被自动删除（因为场景设置了父对象为 this）
}
void wallpaerWidget::setPathFirst(const QString &filePath1, const QString &filePath2)
{
    setBackground(filePath1,m_player_1.get());
    m_player_1->showPlayer(ptype);
    setBackground(filePath2,m_player_2.get());
    m_player_2->hidePlayer(PlayerType::NONPLAYER);
    // 设置当前文件名为第二个文件的文件名
    QFileInfo info(filePath2);
    m_currentFile = info.fileName();
}

void wallpaerWidget::stop()
{

    if(is_player_1)
    {
        m_player_1->stop();
    }
    else
    {
        m_player_2->stop();
    }
}


void wallpaerWidget::play()
{
    qDebug() << "play";
    if(is_player_1)
    {
        m_player_1->play();
        qDebug() << "1 --- play";
    }
    else
    {
        m_player_2->play();
        qDebug() << "1 --- play";
    }
}

void wallpaerWidget::pause()
{
    if(is_player_1)
    {
        m_player_1->pause();
    }
    else
    {
        m_player_2->pause();

    }
}

void wallpaerWidget::setPath(const QString &filePath)
{
    if(is_player_1)
    {
        m_player_2->showPlayer(ptype);

        setBackground(filePath,m_player_1.get());
        m_player_1->hidePlayer(PlayerType::NONPLAYER);
        is_player_1 = false;
    }
    else
    {
        m_player_1->showPlayer(ptype);

        setBackground(filePath,m_player_2.get());
        m_player_2->hidePlayer(PlayerType::NONPLAYER);
        is_player_1 = true;
    }
    // 只存储文件名，而不是完整路径
    QFileInfo info(filePath);
    m_currentFile = info.fileName();

}

void wallpaerWidget::setBackground(const QString &filePath1,Player * player)
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

    resizeEvent();
}


const QString &wallpaerWidget::getFile() const
{
    return m_currentFile;
}

void wallpaerWidget::resizeEvent()
{
    // 调整两个播放器的视频项和图片项大小
    auto adjustPlayer = [this](Player* player) {
        if (!player) return;

        // 调整视频项大小
        auto videoItem = player->getVideoItem();
        if (videoItem) {
            // 设置宽高比模式
            videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
            // 然后设置目标矩形仍为视图大小
            videoItem->setSize(zAxis_Ctrl->getQRect().size());

        }

        // 调整图片项大小
        auto pixmapItem = player->getPixmapItem();
        if (pixmapItem) {
            QPixmap pix = pixmapItem->pixmap();
            if (!pix.isNull()) {
                pixmapItem->setScale(qreal(zAxis_Ctrl->getQRect().width()) / pix.width());
            }
        }
    };

    adjustPlayer(m_player_1.get());
    adjustPlayer(m_player_2.get());
}

