#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout> // 垂直布局
#include <QHBoxLayout> // 水平布局
#include "online.h"

class Friend : public QWidget {
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUsr(PDU *pdu);

    QString m_strSearchName;

signals:

public slots:
    void showOnline();
    void searchUsr();
private:
    QTextEdit *m_pShowMsgTE;            // 显示信息
    QListWidget *m_pFriendListWidget;   // 显示好友列表
    QLineEdit *m_pInputMsgLE;           // 信息输入框

    QPushButton *m_pDelFriendPB;        // 删除好友按钮
    QPushButton *m_pFlushFriendPB;      // 刷新好友列表
    QPushButton *m_pShowOnlineUsrPB;    // 显式在线好友
    QPushButton *m_pSearchUsrPB;        // 查找用户
    QPushButton *m_pMsgSendPB;          // 发送按钮
    QPushButton *m_pPrivateChatPB;      // 私聊按钮

    Online *m_pOnline;

};

#endif // FRIEND_H
