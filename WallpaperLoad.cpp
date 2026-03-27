#include "WallpaperLoad.h"
#include "wallpaerWidget.h"
#include "zaxiscontrol.h"
#include "Data.h"
#include <QDir>
#include <QTimer>
#include <QDebug>
WallpaperLoad::WallpaperLoad(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject(parent)
    , _stime(carMusicSysconfig::WALLPAPER_SWITCH_INTERVAL_MS)
    , _switchTime(new QTimer(this))
    , _wallpaper(nullptr)
    , zAxisCtrl(zAxis_Ctrl)
{
    // _switchTime 的 parent 绑定为 this，避免手动 delete 与 QObject 自动回收冲突。
    // _wallpaper 仅保存外部对象引用，不拥有对象生命周期。
    qDebug() << "WallpaperLoad::ctor" << this << "parent" << parent << "zAxisCtrl" << zAxisCtrl;
    connect(_switchTime, &QTimer::timeout, this, &WallpaperLoad::switchWallpaper);
    connect(zAxisCtrl,&zAxisControl::wallpaperStart,this,&WallpaperLoad::start);
    connect(zAxisCtrl,&zAxisControl::wallpaperPause,this,&WallpaperLoad::pause);
    connect(zAxisCtrl,&zAxisControl::wallpaperStop,this,&WallpaperLoad::stop);
}

WallpaperLoad::~WallpaperLoad()
{
    qDebug() << "WallpaperLoad::dtor" << this << "_wallpaper" << _wallpaper << "_switchTime" << _switchTime;
}

void WallpaperLoad::setwallpaerWidget(wallpaerWidget *widget)
{
    // 仅记录引用，不接管所有权；
    // 真实析构由 MainWindow 生命周期统一管理。
    _wallpaper = widget;
    qDebug() << "WallpaperLoad::setwallpaerWidget" << this << "widget" << _wallpaper;
}

void WallpaperLoad::setPath(const QString &path)
{
    _path = path;
    qDebug() << "WallpaperLoad::setPath" << path << "_wallpaper" << _wallpaper;
    // 扫描目录中所有文件作为轮播候选列表。
    // 这里不限制后缀，实际播放时由 wallpaerWidget::setBackground 决定处理方式。
    QDir dir(_path);
    _wallpapers = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    qDebug() << "WallpaperLoad::setPath wallpapers count" << _wallpapers.size();
    // 首次初始化时一次性给双播放器喂两张素材：
    // 前台先显示第1张，后台预加载第2张，后续切换可做到无缝展示。
    if(!_wallpapers.isEmpty() && _wallpaper)
    {
        if(_wallpapers.size() >= 2)
        {
            _wallpaper->setPathFirst(_path + _wallpapers[0],_path + _wallpapers[1]);
        }
        else
        {
            _wallpaper->setPathFirst(_path + _wallpapers[0],_path + _wallpapers[0]);
        }

        _switchTime->start(_stime);
    }
}

void WallpaperLoad::stop()
{
    qDebug() << "WallpaperLoad::stop";
    if (_switchTime) _switchTime->stop();
    if (_wallpaper) _wallpaper->stop();
}

void WallpaperLoad::start()
{
    qDebug() << "WallpaperLoad::start";
    if (_switchTime) _switchTime->start(_stime);
    if (_wallpaper) _wallpaper->play();
}

void WallpaperLoad::pause()
{
    qDebug() << "WallpaperLoad::pause";
    if (_switchTime) _switchTime->stop();
    if (_wallpaper) _wallpaper->pause();
}

wallpaerWidget* WallpaperLoad::getwallpaerWidget() const
{
    return _wallpaper;
}

void WallpaperLoad::switchWallpaper()
{
    if(_wallpaper && !_wallpapers.isEmpty())
    {
        qDebug() << "WallpaperLoad::switchWallpaper list" << _wallpapers;
        QString filename = _wallpaper->getFile();
        qDebug() << "WallpaperLoad::switchWallpaper current file" << filename;
        // 根据当前正在展示的文件名，找到它在列表中的下一个项作为目标壁纸。
        // 若当前是列表最后一项，则回绕到第一个，形成循环轮播。
        bool is_true = false;//判断是否找到当前壁纸
        for(auto name : _wallpapers)
        {
            if(is_true)
            {
                _wallpaper->setPath(_path + name);
                is_true = false;
            }
            if(name == filename) is_true = true;
        }
        if(is_true) _wallpaper->setPath(_path + _wallpapers[0]);
    }
}
