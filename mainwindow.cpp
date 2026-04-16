#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "WallpaperLoad.h"
#include "wallpaerWidget.h"
#include "desktop.h"
#include "zaxiscontrol.h"
#include <QStatusBar>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

namespace {
constexpr const char *kProjectRootName = "carMusicSys";
constexpr const char *kWallpaperDirSuffix = "D:/code/github/car-music-sys/Wallpaper/";
constexpr const char *kAppIconPath = ":/Resource/app/common/app_icon.jpg";
constexpr const char *kOverlayDesktop = "dosktop";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , zAxisCtrl(new zAxisControl(this))
    , wallpaper(new WallpaperLoad(zAxisCtrl,this))
{
    // 获取Wallpaper目录路径
    path = kWallpaperDirSuffix;
    
    ui->setupUi(this);

    //设置窗口属性
    setMainWindow();
    //设置壁纸
    setWallpaper();

    //添加窗口界面
    addWdiget();
}

MainWindow::~MainWindow()
{
    // 退出前先停止壁纸切换，避免退出事件流中继续触发播放器/场景访问。
    if (wallpaper) {
        wallpaper->stop();
    }
    delete ui;
}

void MainWindow::setMainWindow()
{
    //固定窗口大小不能拖动更改
    // setFixedSize(QSize(1000,800));
    // 隐藏状态栏
    statusBar()->hide();
    //设置标题样式
    // 使用 '|' 组合多个标志
    // setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QIcon icon(QString::fromUtf8(kAppIconPath));
    setWindowIcon(icon);
    setWindowTitle("车载音乐");

    // //设置标题字体,隐藏原标题自定义,需要建立新的类继承QWidget
    // QLabel *titleLabel = new QLabel("车载音乐系统");
    // QFont titleFont("font: 9pt 华文隶书;color: rgb(85, 255, 255);");
    // titleLabel->setFont(titleFont);
}
void MainWindow::setWallpaper()
{
    // 先设置zAxisCtrl为中央部件，这样它的大小会被调整为窗口大小
    //zAxisCtrl是QGraphicsView的子对象
    setCentralWidget(zAxisCtrl);

    // 启动壁纸切换
    wallpaper->setPath(path);
}
void MainWindow::addWdiget()
{
    //桌面控制,只负责app布局
    zAxisCtrl->addOvrlay(QString::fromUtf8(kOverlayDesktop),new desktop(zAxisCtrl),true);
}
