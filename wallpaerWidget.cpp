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
    , m_player_1(std::make_unique<VideoWallpaperPlayer>())
    , m_player_2(std::make_unique<VideoWallpaperPlayer>())
    , is_player_1(true)
    , ptype(PlayerType::NONPLAYER)
{
    //设置相关播放器
    connect(zAxisCtrl,&zAxisControl::resized,this,&wallpaerWidget::resizeEvent);
    setPlayer();
}

void wallpaerWidget::setPlayer()
{
    // 初始化双播放器：
    // m_player_1 与 m_player_2 分别承担“前台展示”和“后台预加载”角色，
    // 每次切换后角色互换，从而避免切换时黑帧。
    m_player_1->setWallpaperPlayer(Layer::LAYER_PLAYER_2);
    m_player_2->setWallpaperPlayer(Layer::LAYER_PLAYER_1);

    // 将双播放器的图元都提前挂进场景，切换时只改可见性，不做频繁 add/remove。
    zAxis_Ctrl->getScene()->addItem(m_player_1->getPixmapItem());
    zAxis_Ctrl->getScene()->addItem(m_player_1->getVideoItem());
    zAxis_Ctrl->getScene()->addItem(m_player_2->getPixmapItem());
    zAxis_Ctrl->getScene()->addItem(m_player_2->getVideoItem());

    // 初始时隐藏所有播放器
    m_player_1->hidePlayer(PlayerType::NONPLAYER);
    m_player_2->hidePlayer(PlayerType::NONPLAYER);

    // 视频加载完成后统一按当前场景尺寸调整，避免首次显示尺寸异常。
    auto setupVideoItem = [this](VideoWallpaperPlayer* player) {
        connect(player->getMediaPlayer(), &QMediaPlayer::mediaStatusChanged, this,
                [this, player](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia) {
                        const auto targetSize = zAxis_Ctrl->getQRect().size().toSize();
                        player->setVideoSize(targetSize);
                    }
                });
    };

    setupVideoItem(m_player_1.get());
    setupVideoItem(m_player_2.get());
}

wallpaerWidget::~wallpaerWidget()
{
    // Player 内部图元由 unique_ptr 管理，但图元同时挂在 QGraphicsScene。
    // 析构前先从 scene 移除，避免退出阶段 scene 与 item 双向销毁产生冲突。
    if (!zAxis_Ctrl || !zAxis_Ctrl->getScene()) {
        return;
    }
    auto scene = zAxis_Ctrl->getScene();
    if (m_player_1) {
        if (m_player_1->getPixmapItem()) scene->removeItem(m_player_1->getPixmapItem());
        if (m_player_1->getVideoItem()) scene->removeItem(m_player_1->getVideoItem());
    }
    if (m_player_2) {
        if (m_player_2->getPixmapItem()) scene->removeItem(m_player_2->getPixmapItem());
        if (m_player_2->getVideoItem()) scene->removeItem(m_player_2->getVideoItem());
    }
}
void wallpaerWidget::setPathFirst(const QString &filePath1, const QString &filePath2)
{
    // 首次启动：
    // 1) player1 加载并显示 filePath1
    // 2) player2 预加载 filePath2 但保持隐藏
    // 后续切换时可直接显示已预加载内容，实现更平滑过渡。
    setBackground(filePath1,m_player_1.get());
    m_player_1->showPlayer(m_player_1->getCurrentPlayerType());
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
    if(is_player_1)
    {
        m_player_1->play();

    }
    else
    {
        m_player_2->play();
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
    QFileInfo info(filePath);
    if (m_currentFile == info.fileName()) {
        return;
    }

    VideoWallpaperPlayer* currentPlayer = is_player_1 ? m_player_1.get() : m_player_2.get();
    VideoWallpaperPlayer* standbyPlayer = is_player_1 ? m_player_2.get() : m_player_1.get();

    // 切换顺序：
    // 1) 先展示后台待机播放器（其内容已提前准备）
    // 2) 再隐藏当前前台播放器
    // 3) 在已隐藏的播放器上加载下一资源，作为下一次待机
    // 该顺序用于降低切换瞬间的可见抖动。
    standbyPlayer->showPlayer(standbyPlayer->getCurrentPlayerType());
    currentPlayer->hidePlayer(PlayerType::NONPLAYER);

    setBackground(filePath, currentPlayer);
    currentPlayer->hidePlayer(PlayerType::NONPLAYER);
    is_player_1 = !is_player_1;
    // 只存储文件名，而不是完整路径
    m_currentFile = info.fileName();

}

void wallpaerWidget::setBackground(const QString &filePath1,VideoWallpaperPlayer * player)
{
    QFileInfo info(filePath1);
    QString suffix = info.suffix().toLower();

    // 根据后缀分发到不同播放管线：
    // 图片 -> pixmap，gif -> movie，mp4 -> mediaPlayer。
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg") {
        player->setupPixmap(filePath1, zAxis_Ctrl->getQRect().size().toSize());
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
    // 尺寸未变化则跳过，减少重复缩放和视频 setSize 调用带来的 CPU 开销。
    const auto targetSize = zAxis_Ctrl->getQRect().size().toSize();
    if (!targetSize.isValid()) {
        return;
    }
    if (m_lastSceneSize == targetSize) {
        return;
    }
    m_lastSceneSize = targetSize;

    auto adjustPlayer = [this](VideoWallpaperPlayer* player) {
        if (!player) return;
        const auto type = player->getCurrentPlayerType();
        if (type == PlayerType::VIDEO) {
            player->setVideoSize(m_lastSceneSize);
        } else if (type == PlayerType::PIXMAP) {
            auto pixmapItem = player->getPixmapItem();
            if (!pixmapItem) return;
            player->refreshPixmap(m_lastSceneSize);
            pixmapItem->setScale(1.0);
        }
    };

    adjustPlayer(m_player_1.get());
    adjustPlayer(m_player_2.get());
}

