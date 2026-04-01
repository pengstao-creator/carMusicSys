#include "AppVideoPlayer.h"

#include <QMediaPlayer>
#include <QVideoWidget>

AppVideoPlayer::AppVideoPlayer(QObject *parent)
    : AppMediaPlayerBase(parent)
{
}

AppVideoPlayer::~AppVideoPlayer() = default;

void AppVideoPlayer::setVideoOutput(QVideoWidget *widget)
{
    m_videoWidget = widget;
    m_player->setVideoOutput(widget);
}

QVideoWidget *AppVideoPlayer::videoWidget() const
{
    return m_videoWidget.data();
}
