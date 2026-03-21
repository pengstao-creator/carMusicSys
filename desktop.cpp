#include "desktop.h"
#include "ui_desktop.h"
#include <QTime>
#include <QDate>
#include <QLocale>
#include <QTimer>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QGraphicsBlurEffect>
desktop::desktop(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::desktop)
    , timeclock(new QTimer(this))
{
    ui->setupUi(this);

    //进入密码界面
    windowDesign();
}

desktop::~desktop()
{
    delete ui;
}

void desktop::resizeEvent(QResizeEvent *event)
{

}

void desktop::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

    } else if (event->button() == Qt::RightButton) {
        qDebug() << "右键按下，位置：" << event->pos();
    }
    // 调用基类，以便事件继续传递（如果需要）
    // QWidget::mousePressEvent(event);
}

void desktop::windowDesign()
{
    setTime();

}

void desktop::setTime()
{
    // 创建垂直布局，整体居中
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->time_hms);
    layout->addWidget(ui->time_ymd);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);  // 整体居中
    setLayout(layout);

    ui->time_hms->setStyleSheet(
        "QLabel {"
        "   color: rgb(255, 255, 255);"
        "   font: 700 italic 48pt 'Segoe UI', 'Microsoft YaHei';"
        "   background: transparent;"
        "   border: none;"
        "   qproperty-alignment: AlignCenter;"
        "}"
        );
    ui->time_ymd->setStyleSheet(
        "QLabel {"
        "   color: rgba(255, 255, 255, 0.85);"
        "   font: 400 12pt 'Segoe UI', 'Microsoft YaHei';"
        "   background: transparent;"
        "   border: none;"
        "   qproperty-alignment: AlignCenter;"
        "}"
        );

    connect(timeclock,&QTimer::timeout,this,&desktop::getTime);
    getTime();
    timeclock->start(1000*1);
}

void desktop::getTime()
{
    // 获取当前本地日期

    QDate currentDate = QDate::currentDate();
    QLocale locale(QLocale::Chinese);
    ui->time_ymd->setText(currentDate.toString("MM月-dd日,") + locale.dayName(currentDate.dayOfWeek()));
    // 获取当前本地时间
    QTime currentTime = QTime::currentTime();
    ui->time_hms->setText(currentTime.toString("hh:mm"));


}
