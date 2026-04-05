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

QString MainWindow::getWallpaperPath()
{
    // 运行目录在 QtCreator 下通常是 build-*/debug 或 build-*/release，
    // 而壁纸目录在源码根目录（carMusicSys/Wallpaper）。
    // 这里从 applicationDirPath 开始逐级向上查找，兼容不同启动位置。
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 8; ++i) {
        // 情况1：当前目录直接包含 Wallpaper（例如手工部署后的目录结构）
        if (dir.exists("Wallpaper")) {
            return QDir::cleanPath(dir.absoluteFilePath("Wallpaper")) + "/";
        }
        // 情况2：当前目录上层包含 carMusicSys/Wallpaper（开发构建目录最常见）
        if (dir.exists(QString::fromUtf8(kProjectRootName) + QString::fromUtf8(kWallpaperDirSuffix))) {
            return QDir::cleanPath(dir.absoluteFilePath(QString::fromUtf8(kProjectRootName) + QString::fromUtf8(kWallpaperDirSuffix))) + "/";
        }
        if (!dir.cdUp()) {
            break;
        }
    }
    // 兜底：保持旧行为，返回 applicationDirPath + /Wallpaper/
    return QCoreApplication::applicationDirPath() + QString::fromUtf8(kWallpaperDirSuffix);
}

MainWindow::~MainWindow()
{
    // 先停止壁纸轮播和播放器，再销毁 UI。
    // 这里显式释放 wallpaerWidget 是为了确保其析构先于 zAxisControl 场景销毁，
    // 避免图元仍挂在场景中导致退出阶段的未定义行为。
    if (wallpaper) {
        wallpaper->stop();
        auto widget = wallpaper->getwallpaerWidget();
        if (widget) {
            // 先断开 WallpaperLoad 对外部对象的引用，再 delete 实例
            wallpaper->setwallpaerWidget(nullptr);
            delete widget;
        }
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

    // 再创建 wallpaerWidget，并交给 WallpaperLoad 统一调度切换逻辑。
    // wallpaerWidget 内部维护双播放器（前台显示 + 后台预加载）。
    auto widget = new wallpaerWidget(zAxisCtrl, this);
    qDebug() << "MainWindow::setWallpaper widget" << widget << "wallpaper" << wallpaper;
    wallpaper->setwallpaerWidget(widget);

    // 启动壁纸切换
    wallpaper->setPath(path);
    qDebug() << "MainWindow::setWallpaper path" << path;
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
    zAxisCtrl->addOvrlay(QString::fromUtf8(kOverlayDesktop),new desktop(zAxisCtrl),true);
}
