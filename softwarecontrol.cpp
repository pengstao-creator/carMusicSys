#include "softwarecontrol.h"
#include "weatherUi.h"
#include "musicui.h"
#include "Overlay.h"
#include "zaxiscontrol.h"
#include "Data.h"
#include <QString>
softwareControl::softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject{parent}
    , zAxisCtrl(zAxis_Ctrl)
{}

void softwareControl::openSoftware(const QString &name)
{
    emit zAxisCtrl->wallpaperStop();
    const auto &overlays = zAxisCtrl->getOvrlay();
    if (overlays.contains(name)) {
        Overlay *overlay = overlays.value(name);
        if (overlay) {
            overlay->setGeometry(zAxisCtrl->getQRect());
            overlay->show();
        }
        return;
    }

    if(name == carMusicSysconfig::APP_WEATHER)
    {
        auto weatherApp = new WeatherUi();
        zAxisCtrl->addOverlay(carMusicSysconfig::APP_WEATHER, weatherApp);
        connect(weatherApp,&WeatherUi::exit,this,[this](){
            const auto &overlays = zAxisCtrl->getOvrlay();
            if (overlays.contains(carMusicSysconfig::APP_WEATHER)) {
                Overlay *overlay = overlays.value(carMusicSysconfig::APP_WEATHER);
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
        zAxisCtrl->addOverlay(carMusicSysconfig::APP_QQMUSIC, musicApp);
        connect(musicApp,&musicUi::exit,this,[this](){
            const auto &overlays = zAxisCtrl->getOvrlay();
            if (overlays.contains(carMusicSysconfig::APP_WEATHER)) {
                Overlay *overlay = overlays.value(carMusicSysconfig::APP_WEATHER);
                if (overlay) {
                    overlay->hide();
                    emit zAxisCtrl->wallpaperStart();
                }
            }
        });
    }
    else if(name == carMusicSysconfig::APP_AMAP)
    {

    }
    else if(name == carMusicSysconfig::APP_BILIBILI)
    {

    }



}



