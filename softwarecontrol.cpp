#include "softwarecontrol.h"
#include "weatherUi.h"
#include "zaxiscontrol.h"
#include "Data.h"
#include <QString>
softwareControl::softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject{parent}
    , zAxisCtrl(zAxis_Ctrl)
{}

void softwareControl::openSoft(const QString &softName)
{
    if(softName == carMusicSysconfig::APP_WEATHER)
    {
        openWeather();
    }
}

void softwareControl::openWeather()
{
    emit zAxisCtrl->wallpaperPause();
    auto weatherApp = new WeatherUi();
    zAxisCtrl->addOverlay(carMusicSysconfig::OVERLAY_WEATHER,weatherApp);
    connect(weatherApp,&WeatherUi::exit,this,[this]()
            {
        zAxisCtrl->wallpaperStart();
    });
}
