#include "weather.h"
#include "ui_weather.h"

weather::weather(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::weather)
{
    ui->setupUi(this);
    // 设置允许浮动、移动和关闭
    setFeatures(QDockWidget::DockWidgetFloatable |
                            QDockWidget::DockWidgetMovable |
                            QDockWidget::DockWidgetClosable);
}

weather::~weather()
{
    delete ui;
}
