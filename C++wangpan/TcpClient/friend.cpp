#include "friend.h"

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
    m_pSearchUsrPB = new QPushButton("查找好友");           // 查找用户
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
}

void Friend::showOnline() {
    if (m_pOnline->isHidden()) { // 如果隐藏就显示
        m_pOnline->show();
    } else {                     // 如果显示就隐藏
        m_pOnline->hide();
    }
}
