#include "online.h"
#include "ui_online.h"

/*
 *
 * 在线用户界面设计
 *
 */

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}
