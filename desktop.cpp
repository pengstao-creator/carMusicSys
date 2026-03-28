#include "desktop.h"
#include "ui_desktop.h"
#include "zaxiscontrol.h"
#include "softwarecontrol.h"
#include <QTime>
#include <QDate>
#include <QLocale>
#include <QTimer>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSizePolicy>
#include <QDebug>

namespace {
constexpr int kClockTickIntervalMs = 1000 * 60;
}

desktop::desktop(zAxisControl * zAxis_Ctrl,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::desktop)
    , timeclock(new QTimer(this))
    , timeHmsLabel(nullptr)
    , timeYmdLabel(nullptr)
    , zAxisCtrl(zAxis_Ctrl)
    , softCtrl(new softwareControl(zAxisCtrl,this))
    , nextAppButtonIndex(0)

{
    ui->setupUi(this);
    ui->rootLayout->setStretch(0, 3);
    ui->rootLayout->setStretch(1, 2);
    ui->rootLayout->setStretch(2, 3);
    timeHmsLabel = ui->timeHmsLabel;
    timeYmdLabel = ui->timeYmdLabel;
    appButtons = { ui->leftApp1, ui->leftApp2, ui->leftApp3, ui->leftApp4, ui->rightApp1, ui->rightApp2, ui->rightApp3 };
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
    softCtrl->setupDesktopApps(this);
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
    timeclock->start(kClockTickIntervalMs);
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



void desktop::addApp(const QString &softName, const QIcon &icon)
{
    if (nextAppButtonIndex < 0 || nextAppButtonIndex >= appButtons.size()) {
        return;
    }
    QPushButton *button = appButtons[nextAppButtonIndex++];
    if (!button) {
        return;
    }
    button->setText(QString());
    button->setStyleSheet("QPushButton{border:none;background:transparent;} QPushButton:pressed{background:rgba(255,255,255,25);}");
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button->setMinimumSize(72, 72);
    button->setIcon(icon);
    button->setEnabled(!icon.isNull()); 
    connect(button,&QPushButton::clicked,this,[this,softName](){
        softCtrl->openSoftware(softName);
    });
    updateIconButtonSizes();
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

    ui->rightGridLayout->setRowStretch(0, 1);
    ui->rightGridLayout->setRowStretch(1, 1);
    ui->rightGridLayout->setColumnStretch(0, 1);
    ui->rightGridLayout->setColumnStretch(1, 1);

    nextAppButtonIndex = 0;
    for (QPushButton *button : appButtons) {
        if (!button) {
            continue;
        }
        button->setIcon(QIcon());
        button->setEnabled(false);
    }
    updateIconButtonSizes();
}

void desktop::updateIconButtonSizes()
{
    for (QPushButton *button : appButtons) {
        const int side = qMax(24, qMin(button->width(), button->height()) - 16);
        button->setIconSize(QSize(side, side));
    }
}

