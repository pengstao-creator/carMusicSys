#include "Player.h"
#include "Data.h"
#include <QUrl>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsVideoItem>
#include <QMovie>
#include <QMediaPlayer>
#include <QSize>
#include <QHash>

// 条件编译，适配Qt5和Qt6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QAudioOutput>
#else
    // Qt5不需要单独包含QAudioOutput
#endif

namespace {
QHash<QString, QPixmap> g_originalPixmapCache;
QHash<QString, QPixmap> g_scaledPixmapCache;

QString buildScaledCacheKey(const QString &path, const QSize &targetSize)
{
    return QString("%1|%2x%3").arg(path).arg(targetSize.width()).arg(targetSize.height());
}
}

Player::Player(QObject *parent)
    : QObject(parent)
    , m_pixmapItem(nullptr)
    , m_videoItem(nullptr)
    , m_movie(nullptr)
    , m_MovieItem(nullptr)
    , m_mediaPlayer(nullptr)
    , ptype(PlayerType::NONPLAYER)
    , m_lastVideoSize()
    , m_videoSignalsConnected(false)
    , m_movieSignalsConnected(false)
{
}

Player::~Player()
{
    // 智能指针会自动管理内存，不需要手动释放
}

void Player::setWallpaperPlayer(const qreal z = Layer::LAYER_PLAYER_1)
{
    setPixmapPlayer(z);
    setVideoPlayer(z);



    // 初始隐藏所有播放器
    hidePlayer(PlayerType::NONPLAYER);
}

void Player::setPixmapPlayer(const qreal z)
{
    if (!m_pixmapItem) {
        m_pixmapItem = std::make_unique<QGraphicsPixmapItem>();
    }
    m_pixmapItem->setZValue(z);          // 底层
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);

}

void Player::setVideoPlayer(const qreal z)
{
    if (!m_videoItem) {
        m_videoItem = std::make_unique<QGraphicsVideoItem>();
    }
    if (!m_mediaPlayer) {
        m_mediaPlayer = std::make_unique<QMediaPlayer>();
    }
    setAudioPlayer();
    // 设置视频输出到该图形项
    m_videoItem->setZValue(z);
    
    // 条件编译以支持Qt5
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_audioOutput->setMuted(true);
    m_audioOutput->setVolume(0.0);
    m_mediaPlayer->setAudioOutput(m_audioOutput.get());    // Qt6 方式连接音频
    #else
    // Qt5使用setVideoOutput直接连接
    #endif
    
    m_mediaPlayer->setVideoOutput(m_videoItem.get());

    // 循环播放
    if (!m_videoSignalsConnected) {
        connect(m_mediaPlayer.get(), &QMediaPlayer::mediaStatusChanged, this,
                [this](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::EndOfMedia)
                        m_mediaPlayer->play();
                });
        m_videoSignalsConnected = true;
    }

}

void Player::setMoviePlayer(const qreal z)
{
    if (!m_MovieItem) {
        m_MovieItem = std::make_unique<QGraphicsPixmapItem>();
    }
    if (!m_movie) {
        m_movie = std::make_unique<QMovie>();
    }
    if (!m_pixmapItem) {
        m_pixmapItem = std::make_unique<QGraphicsPixmapItem>();
    }
    m_MovieItem->setZValue(z);
    // 连接帧更新信号
    if (!m_movieSignalsConnected) {
        connect(m_movie.get(), &QMovie::frameChanged, this, [this](int /*frame*/) {
            if (m_pixmapItem) {
                QPixmap pix = m_movie->currentPixmap();
                m_pixmapItem->setPixmap(pix);
            }
        });
        m_movieSignalsConnected = true;
    }
}

void Player::setAudioPlayer()
{
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (!m_audioOutput) {
        m_audioOutput = std::make_unique<QAudioOutput>();
    }
    #endif
}

void Player::pause()
{
    if(!m_mediaPlayer)return;
    m_mediaPlayer->pause();
}

void Player::stop()
{
    if(!m_mediaPlayer)return;
    m_mediaPlayer->stop();
}

void Player::play()
{
    if(!m_mediaPlayer)return;
    m_mediaPlayer->play();
}

bool Player::hidePlayer(PlayerType type)
{
    if(type == PlayerType::VIDEO)
    {
        if(m_mediaPlayer)m_mediaPlayer->pause();//清除视频资源，黑屏显示，播放位置重置为0
        if(m_videoItem)m_videoItem->setVisible(false);//设置隐藏

    }
    else if(type == PlayerType::PIXMAP )
    {
        if(m_pixmapItem)m_pixmapItem->hide();
    }
    else if(type == PlayerType::MOVIE)
    {
        if(m_MovieItem)m_MovieItem->hide();
        if(m_movie)m_movie->stop();
    }
    else if(type == PlayerType::NONPLAYER)
    {
        // 隐藏所有类型的播放器
        if(m_mediaPlayer)m_mediaPlayer->pause();
        if(m_videoItem)m_videoItem->setVisible(false);
        if(m_pixmapItem)m_pixmapItem->hide();
        if(m_MovieItem)m_MovieItem->hide();
        if(m_movie)m_movie->stop();
    }
    else
    {
        return false;
    }
    return true;
}

bool Player::showPlayer(PlayerType type)
{
    if(type == PlayerType::VIDEO)
    {
        if(m_videoItem)m_videoItem->setVisible(true);//展示
        if(m_mediaPlayer)m_mediaPlayer->play();
    }
    else if(type == PlayerType::PIXMAP)
    {
        if(m_pixmapItem)m_pixmapItem->show();
    }
    else if (type == PlayerType::MOVIE)
    {
        if(m_MovieItem)m_MovieItem->show();
        if(m_movie)m_movie->start();    
    }
    else
    {
        return false;
    }
    return true;
}



void Player::setupPixmap(const QString &path, const QSize &targetSize)
{
    setPixmapPlayer(Layer::LAYER_PLAYER_1);
    ptype = PlayerType::PIXMAP;
    m_currentPixmapPath = path;
    applyCachedPixmap(path, targetSize);
}

void Player::refreshPixmap(const QSize &targetSize)
{
    if (m_currentPixmapPath.isEmpty()) {
        return;
    }
    applyCachedPixmap(m_currentPixmapPath, targetSize);
}

void Player::applyCachedPixmap(const QString &path, const QSize &targetSize)
{
    QPixmap originalPixmap;
    if (g_originalPixmapCache.contains(path)) {
        originalPixmap = g_originalPixmapCache.value(path);
    } else {
        originalPixmap = QPixmap(path);
        if (originalPixmap.isNull()) {
            return;
        }
        g_originalPixmapCache.insert(path, originalPixmap);
    }

    if (targetSize.isValid()) {
        const QString cacheKey = buildScaledCacheKey(path, targetSize);
        if (!g_scaledPixmapCache.contains(cacheKey)) {
            g_scaledPixmapCache.insert(
                cacheKey,
                originalPixmap.scaled(targetSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        }
        m_pixmapItem->setPixmap(g_scaledPixmapCache.value(cacheKey));
        return;
    }

    m_pixmapItem->setPixmap(originalPixmap);
}

void Player::setupMovie(const QString &path)
{
    setMoviePlayer(Layer::LAYER_PLAYER_1);
    ptype = PlayerType::MOVIE;
    m_movie->setFileName(path);
    if (!m_movie->isValid()) {
        return;
    }

    // 初始用第一帧
    QPixmap pix = m_movie->currentPixmap();
    m_pixmapItem->setPixmap(pix);
    m_movie->start();
}

void Player::setupVideo(const QString &path)
{
    setVideoPlayer(Layer::LAYER_PLAYER_1);
    ptype = PlayerType::VIDEO;
    if (m_currentVideoPath == path) {
        m_mediaPlayer->play();
        return;
    }
    m_currentVideoPath = path;
    m_lastVideoSize = QSize();
    
    // 条件编译以支持Qt5
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_mediaPlayer->setSource(QUrl::fromLocalFile(path));
    #else
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(path));
    #endif
    
    m_mediaPlayer->play();
}

void Player::setVideoSize(const QSize &size)
{
    if (!m_videoItem || !size.isValid()) return;
    if (m_lastVideoSize == size) return;
    m_videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
    m_videoItem->setSize(size);
    m_lastVideoSize = size;
}
