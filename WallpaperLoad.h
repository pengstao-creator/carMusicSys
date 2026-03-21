#ifndef WALLPAPERLOAD_H
#define WALLPAPERLOAD_H
#include <QObject>
#include <QStringList>
#include <memory>
class QTimer;
class BackgroundWidget;

class WallpaperLoad : public QObject
{
    Q_OBJECT
public:
    explicit WallpaperLoad(QObject *parent = nullptr);
    ~WallpaperLoad();
    void setBackgroundWidget(BackgroundWidget *widget);
    void setPath(const QString &path);
    BackgroundWidget* getBackgroundWidget() const; // 获取背景部件的原始指针

private slots:
    void switchWallpaper();

private:
    time_t _stime;
    std::unique_ptr<QTimer> _switchTime;//切换壁纸的时间
    std::unique_ptr<BackgroundWidget> _wallpaper;//外部初始化，以为需要其的父对象为QMainWindow
    QStringList _wallpapers;
    QString _path;
};

#endif // WALLPAPERLOAD_H
