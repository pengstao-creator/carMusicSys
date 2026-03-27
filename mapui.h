#ifndef MAPUI_H
#define MAPUI_H

#include <QWidget>

namespace Ui {
class mapUi;
}

class mapUi : public QWidget
{
    Q_OBJECT

public:
    explicit mapUi(QWidget *parent = nullptr);
    ~mapUi();

signals:
    void exit();

private slots:
    void on_exitButton_clicked();

private:
    Ui::mapUi *ui;
};

#endif // MAPUI_H
