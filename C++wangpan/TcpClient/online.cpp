#include "online.h"
#include "ui_online.h"
#include <QDebug>
#include "tcpclient.h"

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

void Online::on_addFriend_pb_clicked() {
    QListWidgetItem *pItem = ui->online_lw->currentItem();
    QString strPerUsrName = pItem->text();                          // 要加的用户的用户名
    QString strLoginName = TcpClient::getInstance().loginName();    // 自己的用户名
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;              // 封装并发送
    memcpy(pdu->caData, strPerUsrName.toStdString().c_str(), strPerUsrName.size());
    memcpy(pdu->caData + 32, strLoginName.toStdString().c_str(), strLoginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}


