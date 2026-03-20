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
    , m_mediaPlayer(std::make_unique<QMediaPlayer>())
    , m_audioOutput(std::make_unique<QAudioOutput>())
    , ptype(PlayerType::NONPLAYER)
{
}

Player::~Player()
{
    // 智能指针会自动管理内存，不需要手动释放
}

void Player::setPlayer()
{
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
    if(type == PlayerType::VIDEO)
    {
        m_mediaPlayer->stop();//清除视频资源，黑屏显示，播放位置重置为0
        m_videoItem->setVisible(false);//设置隐藏
    }
    else if(type == PlayerType::PIXMAP || type == PlayerType::MOVIE)
    {
        m_pixmapItem->hide();
    }
    else if(type == PlayerType::NONPLAYER)
    {
        // 隐藏所有类型的播放器
        m_mediaPlayer->stop();
        m_videoItem->setVisible(false);
        m_pixmapItem->hide();
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
        m_videoItem->setVisible(true);//展示
    }
    else if(type == PlayerType::PIXMAP || type == PlayerType::MOVIE)
    {
        m_pixmapItem->show();
    }
    else
    {
        return false;
    }
    return true;
}

bool Player::switchPlayer(PlayerType type)
{
    if(type == PlayerType::NONPLAYER || type == ptype)
    {
        //当前类型和需要切换类型相同或者无类型不需要切换
        return false;
    }

    //需要切换先隐藏当前播放器
    hidePlayer(ptype);
    //展示需要的播放器
    showPlayer(type);

    ptype = type;
    return true;
}

void Player::setupPixmap(const QString &path)
{
    QPixmap pix(path);
    if (pix.isNull()) {
        qWarning() << "Failed to load image:" << path;
        return;
    }
    m_pixmapItem->setPixmap(pix);
}

void Player::setupMovie(const QString &path)
{
    m_movie->setFileName(path);
    if (!m_movie->isValid()) {
        qWarning() << "Failed to load GIF:" << path;
        return;
    }

    // 初始用第一帧
    QPixmap pix = m_movie->currentPixmap();
    m_pixmapItem->setPixmap(pix);
    m_movie->start();
}

void Player::setupVideo(const QString &path)
{
    qDebug() << "Loading video:" << path;
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