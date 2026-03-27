#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>

namespace Ui {
class desktop;
}
class wallpaerWidget;
class QLabel;
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

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void on_leftApp1_clicked();

    void on_leftApp2_clicked();

    void on_leftApp3_clicked();

    void on_leftApp4_clicked();
    void on_rightApp1_clicked();
    void on_rightApp2_clicked();
    void on_rightApp3_clicked();

private:
    void openSoft(const QString& softName);
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


};

#endif // DESKTOP_H
