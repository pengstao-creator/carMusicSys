#ifndef WALLPAPERLOAD_H
#define WALLPAPERLOAD_H
#include <QObject>
#include <QStringList>
class QTimer;
class wallpaerWidget;
class zAxisControl;
class WallpaperLoad : public QObject
{
    Q_OBJECT
public:
    explicit WallpaperLoad(zAxisControl * zAxis_Ctrl,QObject *parent = nullptr);
    ~WallpaperLoad();
    void setwallpaerWidget(wallpaerWidget *widget);
    void setPath(const QString &path);
    wallpaerWidget* getwallpaerWidget() const;
    void stop();
    void start();
    void pause();
private slots:
    void switchWallpaper();

private:
    time_t _stime;
    QTimer *_switchTime;
    wallpaerWidget *_wallpaper;
    QStringList _wallpapers;
    QString _path;
    zAxisControl * zAxisCtrl;
};

#endif // WALLPAPERLOAD_H
