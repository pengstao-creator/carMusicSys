#ifndef SOFTWARECONTROL_H
#define SOFTWARECONTROL_H

#include <QObject>
class zAxisControl;
class QString;
class softwareControl : public QObject
{
    Q_OBJECT
public:
    explicit softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent = nullptr);
    void openSoft(const QString& softName);

signals:
private:
    void openWeather();
    zAxisControl * zAxisCtrl;
};

#endif // SOFTWARECONTROL_H
