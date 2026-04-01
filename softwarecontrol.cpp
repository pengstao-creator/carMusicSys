#include "softwarecontrol.h"
#include "weatherUi.h"
#include "musicui.h"
#include "settingui.h"
#include "mapui.h"
#include "videoui.h"
#include "desktop.h"
#include "softwareuibase.h"
#include "Overlay.h"
#include "zaxiscontrol.h"
#include <QIcon>
#include <QString>
#include <QDebug>
softwareControl::softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent)
    : QObject{parent}
    , zAxisCtrl(zAxis_Ctrl)
{}

void softwareControl::setupDesktopApps(desktop *desktopUi)
{
    if (!desktopUi) {
        return;
    }
    desktopUi->addApp(WeatherUi::getSoftname(), QIcon(WeatherUi::getSofticon()));
    desktopUi->addApp(musicUi::getSoftname(), QIcon(musicUi::getSofticon()));
    desktopUi->addApp(mapUi::getSoftname(), QIcon(mapUi::getSofticon()));
    desktopUi->addApp(videoUi::getSoftname(), QIcon(videoUi::getSofticon()));
    desktopUi->addApp(settingUi::getSoftname(), QIcon(settingUi::getSofticon()));

}

softwareUiBase *softwareControl::getSoftWidget(const QString &name)
{
    if(WeatherUi::getSoftname() == name)
    {
        return WeatherUi::getSingleton();
    }
    else if(musicUi::getSoftname() == name)
    {
        return musicUi::getSingleton();
    }
    else if(mapUi::getSoftname() == name)
    {
        return mapUi::getSingleton();
    }
    else if(videoUi::getSoftname() == name)
    {
        return videoUi::getSingleton();
    }
    else if(settingUi::getSoftname() == name)
    {
        return settingUi::getSingleton();
    }
    else
    {
        qDebug() << "Software not found: " << name;
    }

    return nullptr;
}

void softwareControl::openSoftware(const QString &name)
{
    if(zAxisCtrl)
    {
        emit zAxisCtrl->wallpaperStop();
        auto softWidget = getSoftWidget(name);
        if(softWidget == nullptr){return;}
        if(zAxisCtrl->getOverlay(name) == nullptr)
        {
            //添加软件窗口
            zAxisCtrl->addOvrlay(name,softWidget,true);
            connect(softWidget,&softwareUiBase::exit,this,[this,softWidget](){
                zAxisCtrl->wallpaperStart();
                if(softWidget)softWidget->hide();
            });
        }
        else
        {
            softWidget->show();
        }
    }
}



