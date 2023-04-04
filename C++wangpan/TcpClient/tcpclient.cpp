#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpClient::TcpClient(QWidget *parent) : QWidget(parent), ui(new Ui::TcpClient) {
    ui->setupUi(this);
    loadConfig();

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect())); // 关联信号和信号处理函数

    // 连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClient::~TcpClient() {
    delete ui;
}

void TcpClient::loadConfig() { // 初始化
    QFile file(":/client.config");                      // 打开文件
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

void TcpClient::showConnect() { // 信号处理函数
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
}

void TcpClient::on_send_pb_clicked() {
    QString strMsg = ui->lineEdit->text();
    if (!strMsg.isEmpty()) {                                                // 如果信息非空
        PDU *pdu = mkPDU(strMsg.size());                                    // 申请 strMsg.size() 大小的空间
        pdu->uiMsgType = 8888;                                              // 给一个消息类型
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.size());    // 将信息复制进 pdu->caMsg
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);                       // 发送数据
        free(pdu);                                                          // 释放
        pdu = NULL;
    } else {
        QMessageBox::warning(this, "信息发送", "发送的信息不能为空");
    }
}
