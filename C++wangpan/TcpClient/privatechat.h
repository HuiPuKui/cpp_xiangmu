#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget {
    Q_OBJECT
public:
    explicit PrivateChat(QWidget *parent = nullptr);
    ~PrivateChat();

    static PrivateChat &getInstance();
    void setChatName(QString strName);
    void updateMsg(const PDU *pdu);

private slots:
    void on_sendMsg_pb_clicked();

private:
    Ui::PrivateChat *ui;
    QString m_strChatName;  // 聊天的对象名字
    QString m_strLoginName; // 登录的用户名
};

#endif // PRIVATECHAT_H
