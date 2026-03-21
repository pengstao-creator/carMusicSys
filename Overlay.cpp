#include "Overlay.h"
#include "ui_overlay.h"

Overlay::Overlay(QGraphicsItem *parent)
    : QGraphicsProxyWidget(parent)
    , ui(new Ui::Overlay)
{
    // 创建UI窗口
    QWidget *widget = new QWidget();
    ui->setupUi(widget);
    // 设置透明背景
    widget->setAttribute(Qt::WA_TranslucentBackground);
    widget->setStyleSheet("background: transparent;");
    setFlag(ItemIgnoresTransformations); // 保持控件大小不随视图缩放
    // 将 QWidget 设置为代理部件
    setWidget(widget);
}

Overlay::~Overlay()
{
    delete ui;
}
