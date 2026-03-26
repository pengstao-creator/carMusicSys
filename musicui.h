#ifndef MUSICUI_H
#define MUSICUI_H

#include <QWidget>

namespace Ui {
class musicUi;
}

class musicUi : public QWidget
{
    Q_OBJECT
signals :
    void exit();

public:
    explicit musicUi(QWidget *parent = nullptr);
    ~musicUi();

private:
    Ui::musicUi *ui;
};

#endif // MUSICUI_H
