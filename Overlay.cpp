#include "Overlay.h"
#include <QWidget>

Overlay::Overlay(QGraphicsItem *parent)
    : QGraphicsProxyWidget(parent)
{
    // 创建一个空的 QWidget
    QWidget *widget = new QWidget();
    // 设置透明背景
    widget->setAttribute(Qt::WA_TranslucentBackground);
    widget->setStyleSheet("background: transparent;");
    setFlag(ItemIgnoresTransformations); // 保持控件大小不随视图缩放
    // 将 QWidget 设置为代理部件
    setWidget(widget);
}

Overlay::~Overlay()
{
}
