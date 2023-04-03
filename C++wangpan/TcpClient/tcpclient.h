#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>    // 文件操作

namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig(); // 初始化，加载配置
private:
    Ui::TcpClient *ui;
    QString m_strIP;    // 存放 IP
    quint16 m_usPort;   // 存放 端口
};

#endif // TCPCLIENT_H
