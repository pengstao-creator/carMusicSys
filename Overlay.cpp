#include "Overlay.h"
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QWidget>
Overlay::Overlay(QGraphicsItem *parent)
    : QGraphicsProxyWidget(parent)
{}

Overlay::~Overlay()
{
}

void Overlay::addWidget(QWidget *overlyD)
{
    // // 设置透明背景
    overlyD->setAttribute(Qt::WA_TranslucentBackground);
    overlyD->setStyleSheet("background: transparent;");
    setWindowFlags(Qt::FramelessWindowHint);      // 无边框
    // 将 QWidget 设置为代理部件
    setWidget(overlyD);
}


