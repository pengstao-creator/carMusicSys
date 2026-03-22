#ifndef WEATHER_H
#define WEATHER_H

#include <QDockWidget>

namespace Ui {
class weather;
}

class weather : public QDockWidget
{
    Q_OBJECT

public:
    explicit weather(QWidget *parent = nullptr);
    ~weather();

private:
    Ui::weather *ui;
};

#endif // WEATHER_H
