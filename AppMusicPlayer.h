#ifndef APPMUSICPLAYER_H
#define APPMUSICPLAYER_H

#include "AppMediaPlayerBase.h"

class AppMusicPlayer : public AppMediaPlayerBase
{
    Q_OBJECT
public:
    explicit AppMusicPlayer(QObject *parent = nullptr);
    ~AppMusicPlayer() override;
};

#endif // APPMUSICPLAYER_H
