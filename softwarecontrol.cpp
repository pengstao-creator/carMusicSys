#include "softwarecontrol.h"
#include "weatherUi.h"
#include "musicui.h"
#include "settingui.h"
#include "mapui.h"
#include "videoui.h"
#include "Overlay.h"
#include "zaxiscontrol.h"
#include "Data.h"
#include <QString>
#include <QDebug>
softwareControl::softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject{parent}
    , zAxisCtrl(zAxis_Ctrl)
{}

void softwareControl::openSoftware(const QString &name)
{
    qDebug() << "softwareControl::openSoftware" << name << "this" << this << "zAxisCtrl" << zAxisCtrl;
    // 打开应用前统一暂停壁纸，避免天气/音乐浮层显示时底层视频继续消耗资源。
    emit zAxisCtrl->wallpaperStop();
    const auto &overlays = zAxisCtrl->getOvrlay();
    qDebug() << "softwareControl::openSoftware overlays" << overlays.keys();
    if (overlays.contains(name)) {
        // Overlay 已存在则直接复用，避免重复 new 页面导致状态丢失与内存增长。
        Overlay *overlay = overlays.value(name);
        qDebug() << "softwareControl::reuseOverlay" << name << overlay;
        if (overlay) {
            overlay->setGeometry(zAxisCtrl->getQRect());
            overlay->show();
        }
        return;
    }

    if(name == carMusicSysconfig::APP_WEATHER)
    {
        // 首次打开天气：创建 WeatherUi 并挂到独立 overlay 层。
        auto weatherApp = new WeatherUi();
        qDebug() << "softwareControl::create WeatherUi" << weatherApp;
        zAxisCtrl->addOverlay(carMusicSysconfig::APP_WEATHER, weatherApp);
        // 天气页点击退出后，仅隐藏 overlay 并恢复壁纸，不销毁页面实例。
        // 这样二次进入天气页时可复用已有控件状态和已加载资源。
        connect(weatherApp,&WeatherUi::exit,this,[this](){
            qDebug() << "softwareControl::weather exit signal";
            const auto &overlays = zAxisCtrl->getOvrlay();
            if (overlays.contains(carMusicSysconfig::APP_WEATHER)) {
                Overlay *overlay = overlays.value(carMusicSysconfig::APP_WEATHER);
                qDebug() << "softwareControl::weather hide overlay" << overlay;
                if (overlay) {
                    overlay->hide();
                    emit zAxisCtrl->wallpaperStart();
                }
            }
        });
    }
    else if(name == carMusicSysconfig::APP_QQMUSIC)
    {
        auto musicApp = new musicUi();
        qDebug() << "softwareControl::create musicUi" << musicApp;
        zAxisCtrl->addOverlay(carMusicSysconfig::APP_QQMUSIC, musicApp);
        connect(musicApp,&musicUi::exit,this,[this](){
            qDebug() << "softwareControl::music exit signal";
            const auto &overlays = zAxisCtrl->getOvrlay();
            if (overlays.contains(carMusicSysconfig::APP_QQMUSIC)) {
                Overlay *overlay = overlays.value(carMusicSysconfig::APP_QQMUSIC);
                if (overlay) {
                    overlay->hide();
                    emit zAxisCtrl->wallpaperStart();
                }
            }
        });
    }
    else if(name == carMusicSysconfig::APP_SETTING)
    {
        auto settingApp = new settingUi();
        zAxisCtrl->addOverlay(carMusicSysconfig::APP_SETTING, settingApp);
        connect(settingApp,&settingUi::exit,this,[this](){
            const auto &overlays = zAxisCtrl->getOvrlay();
            if (overlays.contains(carMusicSysconfig::APP_SETTING)) {
                Overlay *overlay = overlays.value(carMusicSysconfig::APP_SETTING);
                if (overlay) {
                    overlay->hide();
                    emit zAxisCtrl->wallpaperStart();
                }
            }
        });
    }
    else if(name == carMusicSysconfig::APP_AMAP)
    {
        auto mapApp = new mapUi();
        zAxisCtrl->addOverlay(carMusicSysconfig::APP_AMAP, mapApp);
        connect(mapApp,&mapUi::exit,this,[this](){
            const auto &overlays = zAxisCtrl->getOvrlay();
            if (overlays.contains(carMusicSysconfig::APP_AMAP)) {
                Overlay *overlay = overlays.value(carMusicSysconfig::APP_AMAP);
                if (overlay) {
                    overlay->hide();
                    emit zAxisCtrl->wallpaperStart();
                }
            }
        });
    }
    else if(name == carMusicSysconfig::APP_BILIBILI)
    {
        auto videoApp = new videoUi();
        zAxisCtrl->addOverlay(carMusicSysconfig::APP_BILIBILI, videoApp);
        connect(videoApp,&videoUi::exit,this,[this](){
            const auto &overlays = zAxisCtrl->getOvrlay();
            if (overlays.contains(carMusicSysconfig::APP_BILIBILI)) {
                Overlay *overlay = overlays.value(carMusicSysconfig::APP_BILIBILI);
                if (overlay) {
                    overlay->hide();
                    emit zAxisCtrl->wallpaperStart();
                }
            }
        });
    }



}



