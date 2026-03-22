#ifndef WEATHER_H
#define WEATHER_H

#include <QWidget>

namespace Ui {
class weather;
}

class weather : public QWidget
{
    Q_OBJECT

public:
    explicit weather(QWidget *parent = nullptr);
    ~weather();

private:
    Ui::weather *ui;
};

#endif // WEATHER_H
