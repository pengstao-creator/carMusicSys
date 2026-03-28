#ifndef SOFTWARECONTROL_H
#define SOFTWARECONTROL_H

#include <QObject>
class desktop;
class zAxisControl;
class QString;
class QWidget;
class softwareUiBase;
class softwareControl : public QObject
{
    Q_OBJECT
public:
    explicit softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent = nullptr);
    void openSoftware(const QString& name);
    void setupDesktopApps(desktop *desktopUi);

signals:
private:
    softwareUiBase * getSoftWidget(const QString& name);
    zAxisControl * zAxisCtrl;
};

#endif // SOFTWARECONTROL_H
