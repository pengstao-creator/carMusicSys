#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "WallpaperLoad.h"
#include "backgroundwidget.h"
#include "Overlay.h"
#include "desktop.h"
#include <QStatusBar>
#include <QCoreApplication>
#include <QDir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , wallpaper(new WallpaperLoad(this))
{
    // 获取Wallpaper目录路径
    path = getWallpaperPath();
    
    ui->setupUi(this);

    //设置窗口属性
    setMainWindow();
    //设置壁纸
    setWallpaper();

    //添加窗口界面
    addOverlay();
}

QString MainWindow::getWallpaperPath()
{
    // 动态获取Wallpaper目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    // 从appDir中截取到carMusicSys目录
    QString carMusicSysPath = appDir;
    QString targetDir = "carMusicSys";
    int index = carMusicSysPath.indexOf(targetDir);
    if (index != -1) {
        // 截取到carMusicSys目录
        carMusicSysPath = carMusicSysPath.left(index + targetDir.length());
    }
    return carMusicSysPath + "/Wallpaper/";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMainWindow()
{
    //固定窗口大小不能拖动更改
    setFixedSize(QSize(800,400));
    // 隐藏状态栏
    statusBar()->hide();
    //设置标题样式
    // 使用 '|' 组合多个标志
    // setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QIcon icon(":/Resource/5.jpg");
    setWindowIcon(icon);
    setWindowTitle("车载音乐");

    // //设置标题字体,隐藏原标题自定义,需要建立新的类继承QWidget
    // QLabel *titleLabel = new QLabel("车载音乐系统");
    // QFont titleFont("font: 9pt 华文隶书;color: rgb(85, 255, 255);");
    // titleLabel->setFont(titleFont);
}

void MainWindow::setWallpaper()
{
    wallpaper->setBackgroundWidget(new BackgroundWidget(this));
    // 获取BackgroundWidget的原始指针并设置为中央部件
    auto bgWidget = wallpaper->getBackgroundWidget();
    setCentralWidget(bgWidget);
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

void MainWindow::addOverlay()
{
    auto desktopOverlay = new Overlay();
    //桌面控制
    wallpaper->getBackgroundWidget()->addOvrlay({"desktop",desktopOverlay});
    auto Desktop = new desktop();
    BackgroundWidget::getOvrlay()["desktop"]->addWidget(Desktop);
}
