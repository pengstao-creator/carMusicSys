#ifndef VIDEOUI_H
#define VIDEOUI_H

#include <QWidget>

namespace Ui {
class videoUi;
}

class videoUi : public QWidget
{
    Q_OBJECT

public:
    explicit videoUi(QWidget *parent = nullptr);
    ~videoUi();

signals:
    void exit();

private slots:
    void on_exitButton_clicked();

private:
    Ui::videoUi *ui;
};

#endif // VIDEOUI_H
