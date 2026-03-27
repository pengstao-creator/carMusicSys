#include "videoui.h"
#include "ui_videoui.h"

videoUi::videoUi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::videoUi)
{
    ui->setupUi(this);
}

videoUi::~videoUi()
{
    delete ui;
}

void videoUi::on_exitButton_clicked()
{
    close();
    emit exit();
}
