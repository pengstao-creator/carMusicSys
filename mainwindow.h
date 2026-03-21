#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QString>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class WallpaperLoad;
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
    QString getWallpaperPath();
private:
    Ui::MainWindow *ui;
    QString path = "./Wallpaper/";
    std::unique_ptr<WallpaperLoad> wallpaper;

};

#endif // MAINWINDOW_H
