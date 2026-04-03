#include "AppMediaPlayerBase.h"

#include <QMediaPlayer>
#include <QUrl>
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#endif

AppMediaPlayerBase::AppMediaPlayerBase(QObject *parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    , m_audioOutput(new QAudioOutput(this))
#endif
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_player->setAudioOutput(m_audioOutput);
#endif
    connect(m_player, &QMediaPlayer::positionChanged, this, &AppMediaPlayerBase::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AppMediaPlayerBase::durationChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AppMediaPlayerBase::playbackStateChanged);
#else
    connect(m_player, static_cast<void (QMediaPlayer::*)(QMediaPlayer::State)>(&QMediaPlayer::stateChanged), this, [this](QMediaPlayer::State) {
        emit playbackStateChanged();
    });
#endif
}

AppMediaPlayerBase::~AppMediaPlayerBase() = default;

QMediaPlayer *AppMediaPlayerBase::playerHandle() const
{
    return m_player;
}

void AppMediaPlayerBase::setSource(const QString &url)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_player->setSource(QUrl::fromLocalFile(url));
#else
    m_player->setMedia(QUrl::fromLocalFile(url));
#endif
}

void AppMediaPlayerBase::play()
{
    m_player->play();
}

void AppMediaPlayerBase::pause()
{
    m_player->pause();
}

void AppMediaPlayerBase::stop()
{
    m_player->stop();
}

void AppMediaPlayerBase::setVolumePercent(int percent)
{
    const int clamped = qBound(0, percent, 100);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_audioOutput->setVolume(clamped / 100.0);
#else
    m_player->setVolume(clamped);
#endif
}

int AppMediaPlayerBase::volumePercent() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return static_cast<int>(m_audioOutput->volume() * 100.0);
#else
    return m_player->volume();
#endif
}

void AppMediaPlayerBase::setPositionMs(qint64 positionMs)
{
    m_player->setPosition(qMax<qint64>(0, positionMs));
}

qint64 AppMediaPlayerBase::positionMs() const
{
    return m_player->position();
}

qint64 AppMediaPlayerBase::durationMs() const
{
    return m_player->duration();
}
