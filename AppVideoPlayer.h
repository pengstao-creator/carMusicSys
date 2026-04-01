#ifndef APPVIDEOPLAYER_H
#define APPVIDEOPLAYER_H

#include "AppMediaPlayerBase.h"
#include <QPointer>

class QVideoWidget;

class AppVideoPlayer : public AppMediaPlayerBase
{
    Q_OBJECT
public:
    explicit AppVideoPlayer(QObject *parent = nullptr);
    ~AppVideoPlayer() override;

    void setVideoOutput(QVideoWidget *widget);
    QVideoWidget *videoWidget() const;

private:
    QPointer<QVideoWidget> m_videoWidget;
};

#endif // APPVIDEOPLAYER_H
