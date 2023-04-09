#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>        // 文件操作
#include <QTcpSocket>   // 连接服务器、收发服务器数据
#include "protocol.h"
#include "opewidget.h"

namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget {
    Q_OBJECT
public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig(); // 初始化，加载配置

    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    QString loginName();
public slots:           // 槽函数
    void showConnect();
    void recvMsg();
private slots:
//    void on_send_pb_clicked();
    void on_login_pb_clicked();
    void on_regist_pb_clicked();
    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;    // 存放 IP
    quint16 m_usPort;   // 存放 端口

    // 连接服务器，和服务器数据交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;
};

#endif // TCPCLIENT_H
