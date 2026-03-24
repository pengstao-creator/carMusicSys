#include "WallpaperLoad.h"
#include "wallpaerWidget.h"
#include "zaxiscontrol.h"
#include "Data.h"
#include <QDir>
#include <QTimer>

WallpaperLoad::WallpaperLoad(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject(parent)
    , _stime(carMusicSysconfig::WALLPAPER_SWITCH_INTERVAL_MS)
    , _switchTime(std::make_unique<QTimer>(this->parent()))
    , zAxisCtrl(zAxis_Ctrl)
{
    // 设置定时器
    connect(_switchTime.get(), &QTimer::timeout, this, &WallpaperLoad::switchWallpaper);
    connect(zAxisCtrl,&zAxisControl::wallpaperStart,this,&WallpaperLoad::start);
    connect(zAxisCtrl,&zAxisControl::wallpaperPause,this,&WallpaperLoad::pause);
    connect(zAxisCtrl,&zAxisControl::wallpaperStop,this,&WallpaperLoad::stop);
}

WallpaperLoad::~WallpaperLoad()
{
}

void WallpaperLoad::setwallpaerWidget(wallpaerWidget *widget)
{
    _wallpaper.reset(widget);
}

void WallpaperLoad::setPath(const QString &path)
{
    _path = path;
    
    // 获取所有壁纸名称
    QDir dir(_path);
    _wallpapers = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    // 将第一张第二张先添加设为壁纸
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
    _switchTime->stop();
    _wallpaper->stop();
}

void WallpaperLoad::start()
{
    _switchTime->start(_stime);
    _wallpaper->play();
}

void WallpaperLoad::pause()
{
    _switchTime->stop();
    _wallpaper->pause();
}

void WallpaperLoad::switchWallpaper()
{
    if(_wallpaper && !_wallpapers.isEmpty())
    {
        // 切换壁纸
        QString filename = _wallpaper->getFile();
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
