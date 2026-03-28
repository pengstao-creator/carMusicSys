#include "mapui.h"
#include "ui_mapui.h"
#include <QPixmap>
#include <QString>

namespace {
constexpr const char * APP_AMAP = "amap";
constexpr const char * AMAP_ICON = ":/Resource/app/map_app.png";
}

mapUi::mapUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::mapUi)
{
    ui->setupUi(this);
}

mapUi::~mapUi()
{
    delete ui;
}

const QString &mapUi::getSoftname()
{
    static const QString softName = QString::fromUtf8(APP_AMAP);
    return softName;
}

const QPixmap &mapUi::getSofticon()
{
    static const QPixmap softIcon(QString::fromUtf8(AMAP_ICON));
    return softIcon;
}

softwareUiBase *mapUi::getSingleton()
{
    static auto mapApp = new mapUi();
    return mapApp;
}

void mapUi::on_exitButton_clicked()
{
    close();
    emit exit();
}
