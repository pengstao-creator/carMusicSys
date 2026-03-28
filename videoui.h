#ifndef VIDEOUI_H
#define VIDEOUI_H

#include "softwareuibase.h"
class QPixmap;
class QString;

namespace Ui {
class videoUi;
}

class videoUi : public softwareUiBase
{
    Q_OBJECT

public:
    explicit videoUi(QWidget *parent = nullptr);
    ~videoUi();
    static const QString &getSoftname();
    static const QPixmap &getSofticon();
    static softwareUiBase *getSingleton();

private slots:
    void on_exitButton_clicked();

private:
    Ui::videoUi *ui;
};

#endif // VIDEOUI_H
