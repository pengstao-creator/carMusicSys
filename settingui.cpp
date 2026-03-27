#include "settingui.h"
#include "ui_settingui.h"

settingUi::settingUi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::settingUi)
{
    ui->setupUi(this);
}

settingUi::~settingUi()
{
    delete ui;
}

void settingUi::on_exitButton_clicked()
{
    close();
    emit exit();
}
