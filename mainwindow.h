#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class zAxisControl;
class WallpaperLoad;
class Overlay;
class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:


private:
    void setMainWindow();
    void setWallpaper();
    void switchWallpaper();
    void setTime();
    void addWdiget();
    void addOverlay(const QString& name,QWidget * widget);
    QString getWallpaperPath();
private:
    Ui::MainWindow *ui;
    QString path ;
    zAxisControl * zAxisCtrl;
    WallpaperLoad * wallpaper;


};

#endif // MAINWINDOW_H
