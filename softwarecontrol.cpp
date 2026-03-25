#include "softwarecontrol.h"
#include "weatherUi.h"
#include "Overlay.h"
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
    emit zAxisCtrl->wallpaperStop();
    const auto &overlays = zAxisCtrl->getOvrlay();
    if (overlays.contains(carMusicSysconfig::OVERLAY_WEATHER)) {
        Overlay *overlay = overlays.value(carMusicSysconfig::OVERLAY_WEATHER);
        if (overlay) {
            overlay->setGeometry(zAxisCtrl->getQRect());
            overlay->show();
        }
        return;
    }

    auto weatherApp = new WeatherUi();
    zAxisCtrl->addOverlay(carMusicSysconfig::OVERLAY_WEATHER, weatherApp);

    connect(weatherApp,&WeatherUi::exit,this,[this](){
        const auto &overlays = zAxisCtrl->getOvrlay();
        if (overlays.contains(carMusicSysconfig::OVERLAY_WEATHER)) {
            Overlay *overlay = overlays.value(carMusicSysconfig::OVERLAY_WEATHER);
            if (overlay) {
                overlay->hide();
                emit zAxisCtrl->wallpaperStart();
            }
        }
    });

}