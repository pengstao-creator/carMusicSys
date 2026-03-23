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
    emit zAxisCtrl->wallpaperStop();
    auto weatherApp = new weather();
    weatherApp->getweatherForCity("成都");

    zAxisCtrl->addOverlay("weather",weatherApp);
    //绑定退出信号
    connect(weatherApp,&weather::exit,this,[this](){
        //程序退出清理资源
        zAxisCtrl->erase("weather");
        qDebug() << "weather exit" ;
        emit zAxisCtrl->wallpaperStart();
    });

}
