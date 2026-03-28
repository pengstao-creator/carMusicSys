#ifndef DESKTOP_H
#define DESKTOP_H

#include <QHash>
#include <QList>
#include <QWidget>

namespace Ui {
class desktop;
}
class QIcon;
class QLabel;
class QPushButton;
class QResizeEvent;
class QShowEvent;
class QTimer;
class QString;
class zAxisControl;
class softwareControl;
class desktop : public QWidget
{
    Q_OBJECT

public:
    explicit desktop(zAxisControl * zAxis_Ctrl,QWidget *parent = nullptr);
    ~desktop();
    void addApp(const QString &softName, const QIcon &icon);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
private:
    void windowDesign();
    void setTime();
    void getTime(QLabel* ymd, QLabel* hms);
    void setupButtonBaseStyle();
    void setupIconButtons();
    void updateIconButtonSizes();

    Ui::desktop *ui;
    QTimer * timeclock;
    QLabel *timeHmsLabel;
    QLabel *timeYmdLabel;
    zAxisControl * zAxisCtrl;
    softwareControl * softCtrl;
    QList<QPushButton*> appButtons;
    int nextAppButtonIndex;

};

#endif // DESKTOP_H
