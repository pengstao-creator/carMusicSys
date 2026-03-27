#ifndef SETTINGUI_H
#define SETTINGUI_H

#include <QWidget>

namespace Ui {
class settingUi;
}

class settingUi : public QWidget
{
    Q_OBJECT

public:
    explicit settingUi(QWidget *parent = nullptr);
    ~settingUi();

signals:
    void exit();

private slots:
    void on_exitButton_clicked();

private:
    Ui::settingUi *ui;
};

#endif // SETTINGUI_H
