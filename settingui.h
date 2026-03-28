#ifndef SETTINGUI_H
#define SETTINGUI_H

#include "softwareuibase.h"
class QPixmap;
class QString;

namespace Ui {
class settingUi;
}

class settingUi : public softwareUiBase
{
    Q_OBJECT

public:
    explicit settingUi(QWidget *parent = nullptr);
    ~settingUi();
    static const QString &getSoftname();
    static const QPixmap &getSofticon();
    static softwareUiBase *getSingleton();

private slots:
    void on_exitButton_clicked();

private:
    Ui::settingUi *ui;
};

#endif // SETTINGUI_H
