#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include "mytcpserver.h"

namespace Ui {
class TcpServer;
}

class TcpServer : public QWidget {
    Q_OBJECT
public:
    explicit TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig(); // 初始化，加载配置
private:
    Ui::TcpServer *ui;
    QString m_strIP;    // 存放 IP
    quint16 m_usPort;   // 存放 端口
};

#endif // TCPSERVER_H
