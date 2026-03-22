#include "weather.h"
#include "ui_weather.h"

weather::weather(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::weather)
{
    ui->setupUi(this);
}

weather::~weather()
{
    delete ui;
}
