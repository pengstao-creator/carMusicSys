#include "softwarecontrol.h"
#include "weatherUi.h"
#include "zaxiscontrol.h"
#include <QString>
softwareControl::softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject{parent}
    , zAxisCtrl(zAxis_Ctrl)
{}

void softwareControl::openSoft(const QString &softName)
{
    if(softName == "weather")
    {
        openWeather();
    }
}

void softwareControl::openWeather()
{
    emit zAxisCtrl->wallpaperPause();
    auto weatherApp = new WeatherUi();
    zAxisCtrl->addOverlay("weather",weatherApp);
    connect(weatherApp,&WeatherUi::exit,this,[this]()
            {
        zAxisCtrl->wallpaperStart();
    });
}
