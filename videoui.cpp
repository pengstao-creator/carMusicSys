#include "videoui.h"
#include "ui_videoui.h"
#include <QPixmap>
#include <QString>

namespace {
constexpr const char * APP_BILIBILI = "bilibili";
constexpr const char * BILIBILI_ICON = ":/Resource/app/video/icon.png";
}

videoUi::videoUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::videoUi)
{
    ui->setupUi(this);
}

videoUi::~videoUi()
{
    delete ui;
}

const QString &videoUi::getSoftname()
{
    static const QString softName = QString::fromUtf8(APP_BILIBILI);
    return softName;
}

const QPixmap &videoUi::getSofticon()
{
    static const QPixmap softIcon(QString::fromUtf8(BILIBILI_ICON));
    return softIcon;
}

softwareUiBase *videoUi::getSingleton()
{
    static auto videoApp = new videoUi();
    return videoApp;
}

void videoUi::on_exitButton_clicked()
{
    close();
    emit exit();
}
