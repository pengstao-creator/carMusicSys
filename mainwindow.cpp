#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "WallpaperLoad.h"
#include "backgroundwidget.h"
#include <QStatusBar>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , wallpaper(new WallpaperLoad(this))
{
    qDebug() << "MainWindow constructor started";
    ui->setupUi(this);
    qDebug() << "ui->setupUi completed";

    //设置窗口属性
    qDebug() << "Calling setMainWindow()";
    setMainWindow();
    qDebug() << "setMainWindow() completed";
    //设置壁纸
    qDebug() << "Calling setWallpaper()";
    setWallpaper();
    qDebug() << "setWallpaper() completed";
    qDebug() << "MainWindow constructor completed";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMainWindow()
{
    //固定窗口大小不能拖动更改
    // setFixedSize(QSize(800,460));
    // 隐藏状态栏
    statusBar()->hide();
    //设置标题样式
    // 使用 '|' 组合多个标志
    // setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QIcon icon(":/Wallpaper/5.jpg");
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
