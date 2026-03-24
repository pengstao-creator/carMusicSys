#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "WallpaperLoad.h"
#include "wallpaerWidget.h"
#include "desktop.h"
#include "zaxiscontrol.h"
#include "Data.h"
#include <QStatusBar>
#include <QCoreApplication>
#include <QDir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , zAxisCtrl(new zAxisControl(this))
    , wallpaper(new WallpaperLoad(zAxisCtrl,this))
{
    // 获取Wallpaper目录路径
    path = getWallpaperPath();
    
    ui->setupUi(this);

    //设置窗口属性
    setMainWindow();
    //设置壁纸
    setWallpaper();

    //添加窗口界面
    addWdiget();
}

QString MainWindow::getWallpaperPath()
{
    // 动态获取Wallpaper目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    // 从appDir中截取到carMusicSys目录
    QString carMusicSysPath = appDir;
    QString targetDir = carMusicSysconfig::PROJECT_ROOT_NAME;
    int index = carMusicSysPath.indexOf(targetDir);
    if (index != -1) {
        // 截取到carMusicSys目录
        carMusicSysPath = carMusicSysPath.left(index + targetDir.length());
    }
    return carMusicSysPath + carMusicSysconfig::WALLPAPER_DIR_SUFFIX;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMainWindow()
{
    //固定窗口大小不能拖动更改
    // setFixedSize(QSize(800,400));
    // 隐藏状态栏
    statusBar()->hide();
    //设置标题样式
    // 使用 '|' 组合多个标志
    // setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QIcon icon(carMusicSysconfig::APP_ICON_PATH);
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

    // 然后再创建wallpaerWidget，这样它就会使用正确的大小
    wallpaper->setwallpaerWidget(new wallpaerWidget(zAxisCtrl, this));

    // 启动壁纸切换
    wallpaper->setPath(path);
}

void MainWindow::switchWallpaper()
{
    // 壁纸切换逻辑由WallpaperLoad类处理
}

void MainWindow::setTime()
{
    // 时间设置逻辑
}

void MainWindow::addWdiget()
{
    //桌面控制,只负责app布局
    zAxisCtrl->addOverlay(carMusicSysconfig::OVERLAY_DESKTOP,new desktop(zAxisCtrl),true);
}
