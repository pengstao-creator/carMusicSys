#include "Player.h"
#include <QDebug>

#include <QUrl>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsVideoItem>
#include <QMovie>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSize>

Player::Player(QObject *parent)
    : QObject(parent)
    , m_pixmapItem(std::make_unique<QGraphicsPixmapItem>())
    , m_videoItem(std::make_unique<QGraphicsVideoItem>())
    , m_movie(std::make_unique<QMovie>())
    , m_audioOutput(std::make_unique<QAudioOutput>())
    , m_mediaPlayer(std::make_unique<QMediaPlayer>())
    , ptype(PlayerType::NONPLAYER)
{
}

Player::~Player()
{
    // 智能指针会自动管理内存，不需要手动释放
}

void Player::setPlayer()
{
    // 确保所有智能指针都已初始化
    if (!m_pixmapItem || !m_videoItem || !m_movie || !m_mediaPlayer || !m_audioOutput) {
        qDebug() << "Error: Some smart pointers are not initialized";
        return;
    }
    
    m_pixmapItem->setZValue(0);          // 底层
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    
    // 设置视频输出到该图形项
    m_videoItem->setZValue(0);
    m_mediaPlayer->setAudioOutput(m_audioOutput.get());    // Qt6 方式连接音频
    m_mediaPlayer->setVideoOutput(m_videoItem.get());

    // 循环播放
    connect(m_mediaPlayer.get(), &QMediaPlayer::mediaStatusChanged, nullptr, 
            [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia)
                    m_mediaPlayer->play();
            });

    // 连接帧更新信号
    connect(m_movie.get(), &QMovie::frameChanged, nullptr, [this](int /*frame*/) {
        if (m_pixmapItem) {
            QPixmap pix = m_movie->currentPixmap();
            m_pixmapItem->setPixmap(pix);
        }
    });

    // 初始隐藏所有播放器
    hidePlayer(PlayerType::MOVIE);
    hidePlayer(PlayerType::PIXMAP);
    hidePlayer(PlayerType::VIDEO);
}

bool Player::hidePlayer(PlayerType type)
{
    qDebug() << "hidePlayer called with type:" << static_cast<int>(type);
    
    if(type == PlayerType::VIDEO)
    {
        qDebug() << "Hiding VIDEO player";
        m_mediaPlayer->stop();//清除视频资源，黑屏显示，播放位置重置为0
        m_videoItem->setVisible(false);//设置隐藏
    }
    else if(type == PlayerType::PIXMAP || type == PlayerType::MOVIE)
    {
        qDebug() << "Hiding PIXMAP/MOVIE player";
        m_pixmapItem->hide();
    }
    else if(type == PlayerType::NONPLAYER)
    {
        qDebug() << "Hiding ALL players";
        // 隐藏所有类型的播放器
        m_mediaPlayer->stop();
        m_videoItem->setVisible(false);
        m_pixmapItem->hide();
    }
    else
    {
        qDebug() << "Invalid player type";
        return false;
    }
    return true;
}

bool Player::showPlayer(PlayerType type)
{
    qDebug() << "showPlayer called with type:" << static_cast<int>(type);
    
    if(type == PlayerType::VIDEO)
    {
        qDebug() << "Showing VIDEO player";
        m_videoItem->setVisible(true);//展示
    }
    else if(type == PlayerType::PIXMAP || type == PlayerType::MOVIE)
    {
        qDebug() << "Showing PIXMAP/MOVIE player";
        m_pixmapItem->show();
    }
    else
    {
        qDebug() << "Invalid player type";
        return false;
    }
    return true;
}

bool Player::switchPlayer(PlayerType type)
{
    qDebug() << "switchPlayer called with type:" << static_cast<int>(type);
    qDebug() << "Current player type:" << static_cast<int>(ptype);
    
    if(type == PlayerType::NONPLAYER || type == ptype)
    {
        qDebug() << "No switch needed, same type or NONPLAYER";
        //当前类型和需要切换类型相同或者无类型不需要切换
        return false;
    }

    //需要切换先隐藏当前播放器
    qDebug() << "Hiding current player type:" << static_cast<int>(ptype);
    hidePlayer(ptype);
    //展示需要的播放器
    qDebug() << "Showing new player type:" << static_cast<int>(type);
    showPlayer(type);

    ptype = type;
    qDebug() << "Switch completed, new player type:" << static_cast<int>(ptype);
    return true;
}

void Player::setupPixmap(const QString &path)
{
    qDebug() << "setupPixmap called with path:" << path;
    QPixmap pix(path);
    if (pix.isNull()) {
        qDebug() << "Failed to load pixmap";
        return;
    }
    qDebug() << "Pixmap loaded successfully";
    m_pixmapItem->setPixmap(pix);
}

void Player::setupMovie(const QString &path)
{
    qDebug() << "setupMovie called with path:" << path;
    m_movie->setFileName(path);
    if (!m_movie->isValid()) {
        qDebug() << "Failed to load movie";
        return;
    }
    qDebug() << "Movie loaded successfully";

    // 初始用第一帧
    QPixmap pix = m_movie->currentPixmap();
    m_pixmapItem->setPixmap(pix);
    m_movie->start();
    qDebug() << "Movie started";
}

void Player::setupVideo(const QString &path)
{
    qDebug() << "setupVideo called with path:" << path;
    // 确保视频项可见
    if (m_videoItem) {
        m_videoItem->setVisible(true);
        qDebug() << "Video item visibility set to true";
    }
    m_mediaPlayer->setSource(QUrl::fromLocalFile(path));
    m_mediaPlayer->play();
    qDebug() << "Video playback started";
}

void Player::setVideoSize(const QSize &size)
{
    if (m_videoItem) {
        m_videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
        m_videoItem->setSize(size);
    }
}