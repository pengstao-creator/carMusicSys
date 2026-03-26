#ifndef SOFTWARECONTROL_H
#define SOFTWARECONTROL_H

#include <QObject>
class zAxisControl;
class QString;
class QWidget;
class softwareControl : public QObject
{
    Q_OBJECT
public:
    explicit softwareControl(zAxisControl * zAxis_Ctrl,QObject *parent = nullptr);
    void openSoftware(const QString& name);

signals:
private:

    zAxisControl * zAxisCtrl;
};

#endif // SOFTWARECONTROL_H
