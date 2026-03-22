#include "softwarecontrol.h"
#include "weather.h"
#include "zaxiscontrol.h"
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
    auto weatherApp = new weather();
    weatherApp->getweatherForCity("成都");

    zAxisCtrl->addOverlay("weather",weatherApp);
    auto wetherWidget = zAxisCtrl->getOvrlay();

}
