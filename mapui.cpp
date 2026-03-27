#include "mapui.h"
#include "ui_mapui.h"

mapUi::mapUi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mapUi)
{
    ui->setupUi(this);
}

mapUi::~mapUi()
{
    delete ui;
}

void mapUi::on_exitButton_clicked()
{
    close();
    emit exit();
}
