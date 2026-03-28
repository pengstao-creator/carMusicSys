#ifndef MAPUI_H
#define MAPUI_H

#include "softwareuibase.h"
class QPixmap;
class QString;

namespace Ui {
class mapUi;
}

class mapUi : public softwareUiBase
{
    Q_OBJECT

public:
    explicit mapUi(QWidget *parent = nullptr);
    ~mapUi();
    static const QString &getSoftname();
    static const QPixmap &getSofticon();
    static softwareUiBase *getSingleton();

private slots:
    void on_exitButton_clicked();

private:
    Ui::mapUi *ui;
};

#endif // MAPUI_H
