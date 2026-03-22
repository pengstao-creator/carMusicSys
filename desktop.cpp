#include "desktop.h"
#include "ui_desktop.h"
#include "zaxiscontrol.h"
#include "softwarecontrol.h"
#include <QTime>
#include <QDate>
#include <QLocale>
#include <QTimer>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QGraphicsBlurEffect>
#include <QLabel>
desktop::desktop(zAxisControl * zAxis_Ctrl,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::desktop)
    , timeclock(new QTimer(this))
    , timecontainer(new QWidget(this))
    , zAxisCtrl(zAxis_Ctrl)
    , softCtrl(new softwareControl(zAxisCtrl,this))

{
    ui->setupUi(this);
    timecontainer->setGeometry(280,30,200,150);
    setBaseSize(size().width(),size().height());
    //进入密码界面
    windowDesign();

}

desktop::~desktop()
{
    delete ui;
}

void desktop::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if(timecontainer)
    {
        //计算缩放比例
        double wideRatio = (double)size().width() / baseSize().width();
        double highRatio = (double)size().height()/ baseSize().height();
        //从新设置容器大小,按照初始位置进行缩放
        int  newx = 280 * wideRatio;
        int newy= 30 * highRatio;
        int neww = 200 * wideRatio;
        int newh = 150 * highRatio;
        timecontainer->setGeometry(newx,newy,neww,newh);
    }


}



// void desktop::mousePressEvent(QMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton) {

//     } else if (event->button() == Qt::RightButton) {
//         qDebug() << "右键按下，位置：" << event->pos();
//     }
//     // 调用基类，以便事件继续传递（如果需要）
//     QWidget::mousePressEvent(event);
// }

void desktop::windowDesign()
{
    setTime();

}

void desktop::setTime()
{
    // 创建垂直布局，整体居中

    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *time_hms = new QLabel("Label 1");
    QLabel *time_ymd = new QLabel("Label 2");
    layout->addWidget(time_hms);
    layout->addWidget(time_ymd);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignCenter);  // 整体居中
    // //设置拉伸策略
    time_hms->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    time_ymd->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    timecontainer->setLayout(layout);

    time_hms->setStyleSheet(
        "QLabel {"
        "   color: rgb(255, 255, 255);"
        "   font: 700 italic 48pt 'Segoe UI', 'Microsoft YaHei';"
        "   background: transparent;"
        "   border: none;"
        "   qproperty-alignment: AlignCenter;"
        "}"
        );
    time_ymd->setStyleSheet(
        "QLabel {"
        "   color: rgba(255, 255, 255, 0.85);"
        "   font: 400 12pt 'Segoe UI', 'Microsoft YaHei';"
        "   background: transparent;"
        "   border: none;"
        "   qproperty-alignment: AlignCenter;"
        "}"
        );

    connect(timeclock,&QTimer::timeout,this,[time_ymd,time_hms,this](){
        getTime(time_ymd,time_hms);
    });
    getTime(time_ymd,time_hms);
    timeclock->start(1000*1);
}

void desktop::getTime(QLabel* ymd, QLabel* hms)
{
    // 获取当前本地日期

    QDate currentDate = QDate::currentDate();
    QLocale locale(QLocale::Chinese);
    ymd->setText(currentDate.toString("MM月-dd日,") + locale.dayName(currentDate.dayOfWeek()));
    // 获取当前本地时间
    QTime currentTime = QTime::currentTime();
    hms->setText(currentTime.toString("hh:mm"));


}

void desktop::on_weather_clicked()
{
    openSoft("weather");
}


void desktop::on_QQMusic_clicked()
{
    openSoft("QQMusic");
}


void desktop::on_amap_clicked()
{
    openSoft("amap");
}


void desktop::on_bilibili_clicked()
{
    openSoft("bilibili");
}

void desktop::openSoft(const QString &softName)
{
    softCtrl->openSoft(softName);
}

