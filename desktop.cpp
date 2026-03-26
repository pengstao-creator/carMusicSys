#include "desktop.h"
#include "ui_desktop.h"
#include "zaxiscontrol.h"
#include "softwarecontrol.h"
#include "Data.h"
#include <QTime>
#include <QDate>
#include <QLocale>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSizePolicy>
desktop::desktop(zAxisControl * zAxis_Ctrl,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::desktop)
    , timeclock(new QTimer(this))
    , timeHmsLabel(nullptr)
    , timeYmdLabel(nullptr)
    , zAxisCtrl(zAxis_Ctrl)
    , softCtrl(new softwareControl(zAxisCtrl,this))

{
    ui->setupUi(this);
    ui->rootLayout->setStretch(0, 3);
    ui->rootLayout->setStretch(1, 2);
    ui->rootLayout->setStretch(2, 3);
    timeHmsLabel = ui->timeHmsLabel;
    timeYmdLabel = ui->timeYmdLabel;
    windowDesign();
    QTimer::singleShot(0, this, [this]() { updateIconButtonSizes(); });

}

desktop::~desktop()
{
    delete ui;
}

void desktop::windowDesign()
{
    setupButtonBaseStyle();
    setupIconButtons();
    setTime();
}

void desktop::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateIconButtonSizes();
}

void desktop::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateIconButtonSizes();
}

void desktop::setTime()
{
    if (!timeHmsLabel || !timeYmdLabel) {
        return;
    }
    connect(timeclock,&QTimer::timeout,this,[this](){
        getTime(timeYmdLabel,timeHmsLabel);
    });
    getTime(timeYmdLabel,timeHmsLabel);
    timeclock->start(carMusicSysconfig::CLOCK_TICK_INTERVAL_MS);
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
#include <QThread>
void desktop::on_weather_clicked()
{
    qDebug() << "on_weather_clicked" <<  QThread::currentThreadId();
    openSoft(carMusicSysconfig::APP_WEATHER);
}


void desktop::on_QQMusic_clicked()
{
    openSoft(carMusicSysconfig::APP_QQMUSIC);
}


void desktop::on_amap_clicked()
{
    openSoft(carMusicSysconfig::APP_AMAP);
}


void desktop::on_bilibili_clicked()
{
    openSoft(carMusicSysconfig::APP_BILIBILI);
}

void desktop::openSoft(const QString &softName)
{
    softCtrl->openSoftware(softName);
}

void desktop::setupButtonBaseStyle()
{
    const QList<QPushButton*> buttons = findChildren<QPushButton*>();
    for (QPushButton *button : buttons) {
        button->setText(QString());
        button->setFlat(true);
        button->setStyleSheet("QPushButton{border:none;background:transparent;} QPushButton:pressed{background:rgba(255,255,255,25);}");
    }
}

void desktop::setupIconButtons()
{
    ui->leftGridLayout->setRowStretch(0, 1);
    ui->leftGridLayout->setRowStretch(1, 1);
    ui->leftGridLayout->setColumnStretch(0, 1);
    ui->leftGridLayout->setColumnStretch(1, 1);

    const QList<QPushButton*> iconButtons = { ui->weather, ui->QQMusic, ui->amap, ui->bilibili };
    const QList<QString> iconPaths = {
        QString::fromUtf8(carMusicSysconfig::WEATHER_APP_PATH) + "6.png",
        QString::fromUtf8(carMusicSysconfig::WEATHER_APP_PATH) + "2.png",
        QString::fromUtf8(carMusicSysconfig::WEATHER_APP_PATH) + "4.png",
        QString::fromUtf8(carMusicSysconfig::WEATHER_APP_PATH) + "3.png"
    };

    for (int i = 0; i < iconButtons.size(); ++i) {
        QPushButton *button = iconButtons[i];
        button->setStyleSheet("QPushButton{border:none;background:transparent;} QPushButton:pressed{background:rgba(255,255,255,25);}");
        button->setText(QString());
        button->setIcon(QIcon(iconPaths[i]));
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        button->setMinimumSize(72, 72);
    }

    const QList<QPushButton*> buttons = findChildren<QPushButton*>();
    for (QPushButton *button : buttons) {
        const bool hasIcon = !button->icon().isNull();
        button->setEnabled(hasIcon);
    }
    updateIconButtonSizes();
}

void desktop::updateIconButtonSizes()
{
    const QList<QPushButton*> iconButtons = { ui->weather, ui->QQMusic, ui->amap, ui->bilibili };
    for (QPushButton *button : iconButtons) {
        const int side = qMax(24, qMin(button->width(), button->height()) - 16);
        button->setIconSize(QSize(side, side));
    }
}

