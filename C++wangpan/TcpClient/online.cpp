#include "online.h"
#include "ui_online.h"
#include <QDebug>

/*
 *
 * 在线用户界面设计
 *
 */

Online::Online(QWidget *parent) : QWidget(parent), ui(new Ui::Online) {
    ui->setupUi(this);
}

Online::~Online() {
    delete ui;
}

void Online::showUsr(PDU *pdu) {                       // 显示用户
    if (NULL == pdu) {
        return ;
    }
    uint uiSize = pdu->uiMsgLen / 32;

    char caTmp[32];
    for (uint i = 0; i < uiSize; i ++) {                // 依次遍历每个用户名
        memcpy(caTmp, (char*)(pdu->caMsg) + i * 32, 32);
        ui->online_lw->addItem(caTmp);                  // 把用户名显示出来
    }
}
