#include "WallpaperLoad.h"
#include "backgroundwidget.h"
#include <QDir>
#include <QTimer>

WallpaperLoad::WallpaperLoad(QObject *parent)
    : QObject(parent)
    , _stime(1000*1)
    , _switchTime(std::make_unique<QTimer>(this->parent()))
{
    // 设置定时器
    connect(_switchTime.get(), &QTimer::timeout, this, &WallpaperLoad::switchWallpaper);
}

WallpaperLoad::~WallpaperLoad()
{
}

void WallpaperLoad::setBackgroundWidget(BackgroundWidget *widget)
{
    _wallpaper.reset(widget);
}

void WallpaperLoad::setPath(const QString &path)
{
    _path = path;
    
    // 获取所有壁纸名称
    QDir dir(_path);
    _wallpapers = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    qDebug() << _wallpapers ;
    // 将第一张设为壁纸
    if(!_wallpapers.isEmpty() && _wallpaper)
    {
        _wallpaper->setBackground(_path + _wallpapers[0]);
        _switchTime->start(_stime);
    }
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
                _wallpaper->setBackground(_path + name);
                qDebug() << name << "-------";
                is_true = false;
            }
            if(name == filename) is_true = true;
        }
        if(is_true) _wallpaper->setBackground(_path + _wallpapers[0]);
    }
}

BackgroundWidget* WallpaperLoad::getBackgroundWidget() const
{
    return _wallpaper.get();
}
