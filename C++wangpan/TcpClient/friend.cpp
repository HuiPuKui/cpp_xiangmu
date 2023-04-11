#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog> // 专门用来输入数据
#include <QDebug>

/*
 *
 * 好友界面设计
 *
 */

Friend::Friend(QWidget *parent) : QWidget(parent) {       // 窗口设计
    m_pShowMsgTE = new QTextEdit;            // 显示信息
    m_pFriendListWidget = new QListWidget;   // 显式好友列表
    m_pInputMsgLE = new QLineEdit;           // 信息输入框

    m_pDelFriendPB = new QPushButton("删除好友");           // 删除好友按钮
    m_pFlushFriendPB = new QPushButton("刷新好友");         // 刷新好友列表
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");    // 显式在线好友
    m_pSearchUsrPB = new QPushButton("查找用户");           // 查找用户
    m_pMsgSendPB = new QPushButton("信息发送");             // 显式在线好友
    m_pPrivateChatPB = new QPushButton("私聊");            // 查找用户

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();                                      // 先隐藏不显示

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB, SIGNAL(clicked(bool)), this, SLOT(showOnline()));
    connect(m_pSearchUsrPB, SIGNAL(clicked(bool)), this, SLOT(searchUsr()));
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)), this, SLOT(flushFriend()));
}

void Friend::showAllOnlineUsr(PDU *pdu) {
    if (NULL == pdu) {
        return ;
    }
    m_pOnline->showUsr(pdu); // 把 pdu 传给显示函数
}

void Friend::updateFriendList(PDU *pdu) {
    if (NULL == pdu) {
        return ;
    }
    uint uiSize = pdu->uiMsgLen / 32;
    char caName[32] = {'\0'};
    for (uint i = 0; i < uiSize; i ++) {
        memcpy(caName, (char*)(pdu->caMsg) + i * 32, 32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::showOnline() {
    if (m_pOnline->isHidden()) { // 如果隐藏就显示
        m_pOnline->show();

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST; // 封装请求信息
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen); // 请求信息发送
        free(pdu); // 释放
        pdu = NULL;
    } else {                     // 如果显示就隐藏
        m_pOnline->hide();
    }
}

void Friend::searchUsr() { // 搜索用户
    m_strSearchName = QInputDialog::getText(this, "搜索", "用户名：");
    if (!m_strSearchName.isEmpty()) {
        qDebug() << m_strSearchName;
        PDU *pdu = mkPDU(0);
        memcpy(pdu->caData, m_strSearchName.toStdString().c_str(), m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Friend::flushFriend() {
    QString strName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData, strName.toStdString().c_str(), strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}
