#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>

namespace Ui {
class desktop;
}
class wallpaerWidget;
class QLabel;
class QResizeEvent;
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

protected:
    void resizeEvent(QResizeEvent *event) override;
    // void mousePressEvent(QMouseEvent *event) override;
private slots:
    void on_weather_clicked();

    void on_QQMusic_clicked();

    void on_amap_clicked();

    void on_bilibili_clicked();

private:
    void openSoft(const QString& softName);
    void windowDesign();
    void setTime();
    void getTime(QLabel* ymd, QLabel* hms);

    Ui::desktop *ui;
    QTimer * timeclock;
    QWidget * timecontainer;
    zAxisControl * zAxisCtrl;
    softwareControl * softCtrl;


};

#endif // DESKTOP_H
