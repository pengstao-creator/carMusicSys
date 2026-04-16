#include "WallpaperLoad.h"
#include "wallpaerWidget.h"
#include "zaxiscontrol.h"
#include <QDir>
#include <QTimer>
#include <QDebug>

namespace {
constexpr int kWallpaperSwitchIntervalMs = 1000 * 5;
}

WallpaperLoad::WallpaperLoad(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject(parent)
    , _stime(kWallpaperSwitchIntervalMs)
    , _switchTime(new QTimer(this))
    , _wallpaper(new wallpaerWidget(zAxis_Ctrl,this))
    , zAxisCtrl(zAxis_Ctrl)
{
    // _switchTime 的 parent 绑定为 this，避免手动 delete 与 QObject 自动回收冲突。
    // _wallpaper 仅保存外部对象引用，不拥有对象生命周期。
    qDebug() << "WallpaperLoad::ctor" << this << "parent" << parent << "zAxisCtrl" << zAxisCtrl;
    connect(_switchTime, &QTimer::timeout, this, &WallpaperLoad::switchWallpaper);
    connect(zAxisCtrl,&zAxisControl::wallpaperStart,this,&WallpaperLoad::start);
    connect(zAxisCtrl,&zAxisControl::wallpaperPause,this,&WallpaperLoad::pause);
    connect(zAxisCtrl,&zAxisControl::wallpaperStop,this,&WallpaperLoad::stop);
    connect(zAxisCtrl,&zAxisControl::switchWallpaperPath,this,[this](const QString& path){
        setPath(path);
        pause();
    });
    connect(zAxisCtrl,&zAxisControl::switchWpDur,this,[this](int second){
        _stime = second;
    });
}

WallpaperLoad::~WallpaperLoad()
{
    // 先停定时器，避免析构过程中再触发切换回调访问半析构对象。
    if (_switchTime) {
        _switchTime->stop();
    }
    // 明确在 WallpaperLoad 析构阶段释放壁纸对象，确保其先于场景相关对象退出。
    if (_wallpaper) {
        _wallpaper->stop();
        delete _wallpaper;
        _wallpaper = nullptr;
    }
}


void WallpaperLoad::setPath(const QString &path)
{
    QFileInfo filename(path);
    if(!filename.exists() || !_wallpaper)
    {
        emit zAxisCtrl->isPathInvalid();
        return;
    }
    QDir dir;
    if (filename.isDir()) {
        dir = QDir(filename.absoluteFilePath());
    } else {
        dir = filename.dir();
    }
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.gif" << "*.mp4";
    auto filelists = dir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    if(filelists.isEmpty())
    {
        //目录为空或者读取失败
        emit zAxisCtrl->isPathInvalid();
        return;
    }
    //保存文件的绝对路径
    _wallpapers.clear();
    for(const auto& filepath : filelists)
    {
        _wallpapers << dir.absoluteFilePath(filepath);
    }
    qDebug() << "_______________________" <<_wallpapers;
    //获取第一个和第二个文件的绝对路径
    //如果选择的文件,则播放该文件和缓存下一个文件
    QString path1 = _wallpapers[0];
    QString path2;
    if(_wallpapers.size() >= 2)
    {
        path2 = _wallpapers[1];
    }
    else
    {
        path2 = _wallpapers[0];
    }
    if(filename.isFile() && _wallpapers.size() >= 2)
    {
        for(int i = 0;i < _wallpapers.size();i++)
        {
            if(_wallpapers[i] == filename.absoluteFilePath())
            {
                path1 = _wallpapers[i];
                if(i + 1 < _wallpapers.size())
                {
                    path2 = _wallpapers[i + 1];
                }
                else
                {
                    path2 = _wallpapers[0];
                }
            }
        }
    }
    // 首次初始化时一次性给双播放器喂两张素材：
    // 前台先显示第1张，后台预加载第2张，后续切换可做到无缝展示。
    _wallpaper->setPathFirst(path1,path2);
    _switchTime->start(_stime);
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
        QString filename = _wallpaper->getFile();
        // 根据当前正在展示的文件名，找到它在列表中的下一个项作为目标壁纸。
        // 若当前是列表最后一项，则回绕到第一个，形成循环轮播。
        bool is_true = false;//判断是否找到当前壁纸
        for(auto name : _wallpapers)
        {
            if(is_true)
            {
                _wallpaper->setPath(name);
                qDebug() << "开始播放" << name;
                is_true = false;
            }
            if(name == filename) is_true = true;
        }
        if(is_true) _wallpaper->setPath(_wallpapers[0]);
    }
}
