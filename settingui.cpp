#include "settingui.h"
#include "ui_settingui.h"
#include <QPixmap>
#include <QString>

namespace {
constexpr const char * APP_SETTING = "setting";
constexpr const char * SETTING_ICON = ":/Resource/app/settings_app.png";
}

settingUi::settingUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::settingUi)
{
    ui->setupUi(this);
}

settingUi::~settingUi()
{
    delete ui;
}

const QString &settingUi::getSoftname()
{
    static const QString softName = QString::fromUtf8(APP_SETTING);
    return softName;
}

const QPixmap &settingUi::getSofticon()
{
    static const QPixmap softIcon(QString::fromUtf8(SETTING_ICON));
    return softIcon;
}

softwareUiBase *settingUi::getSingleton()
{
    static auto settingApp = new settingUi();
    return settingApp;
}

void settingUi::on_exitButton_clicked()
{
    close();
    emit exit();
}
