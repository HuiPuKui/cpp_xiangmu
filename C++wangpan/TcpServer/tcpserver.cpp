#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>
#include <QDebug>

TcpServer::TcpServer(QWidget *parent) : QWidget(parent), ui(new Ui::TcpServer) {
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort); // 监听、只要有客户端连接到服务器 incomingconnection 就会自动调用
}

TcpServer::~TcpServer() {
    delete ui;
}

void TcpServer::loadConfig() {
    QFile file(":/server.config");                      // 打开文件
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray baData = file.readAll();             // 全部读取出来
        QString strData = baData.toStdString().c_str(); // 转换成 char * 型
        file.close();                                   // 关闭文件

        strData.replace("\r\n", " ");                   // 将 "\r\n" 替换成 " "

        QStringList strList = strData.split(" ");       // 以 " " 分割，返回 QString 列表

        m_strIP = strList.at(0);                        // 列表 0 位置为 IP
        m_usPort = strList.at(1).toUShort();            // 列表 1 位置为 端口号
    } else {
        QMessageBox::critical(this, "open config", "open config failed"); // 打不开文件
    }
}
