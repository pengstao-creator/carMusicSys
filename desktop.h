#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>

namespace Ui {
class desktop;
}
class BackgroundWidget;
class desktop : public QWidget
{
    Q_OBJECT

public:
    explicit desktop(QWidget *parent = nullptr);
    ~desktop();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    void windowDesign();
    void setTime();
    void getTime();

    Ui::desktop *ui;
    QTimer * timeclock;
};

#endif // DESKTOP_H
