#include "musicui.h"
#include "ui_musicui.h"

musicUi::musicUi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::musicUi)
{
    ui->setupUi(this);
}

musicUi::~musicUi()
{
    delete ui;
}
