#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpClient::TcpClient(QWidget *parent) : QWidget(parent), ui(new Ui::TcpClient) {
    ui->setupUi(this);

    resize(500, 250);

    loadConfig();

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect())); // 关联信号和信号处理函数
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(recvMsg()));     // 用 recvMsg 来接收服务器传回来的信息
    // 连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClient::~TcpClient() {
    delete ui;
}

void TcpClient::recvMsg() {
    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));                        // 首先读 sizeof(uint) 大小的字节，将 uiPDULen 读进来
    uint uiMsgLen = uiPDULen - sizeof(PDU);                                  // 那么实际消息长度就是总的协议数据单元大小减去 sizeof(PDU)
    PDU *pdu = mkPDU(uiMsgLen);                                              // 因此在弹性结构体申请 uiMsgLen 大小的空间进行接收
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));    // 接收要在上个接受的之后，所以加一个偏移量
//    qDebug() << pdu->caData << ' ' << pdu->uiMsgType;
//    qDebug() << ENUM_MSG_TYPE_MIN << ' ' << ENUM_MSG_TYPE_REGIST_REQUEST << ' ' << ENUM_MSG_TYPE_REGIST_RESPOND;
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGIST_RESPOND: {                                     // 接收到注册返回信息
        if (0 == strcmp(pdu->caData, REGIST_OK)) {                           // 注册成功
            QMessageBox::information(this, "注册", REGIST_OK);
        } else if (0 == strcmp(pdu->caData, REGIST_FAILED)) {                // 注册失败
            QMessageBox::warning(this, "注册", REGIST_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND: {                                     // 接收到登录返回信息
        if (0 == strcmp(pdu->caData, LOGIN_OK)) {                           // 登录成功
            QMessageBox::information(this, "登录", LOGIN_OK);
        } else if (0 == strcmp(pdu->caData, LOGIN_FAILED)) {                // 登录失败
            QMessageBox::warning(this, "登录", LOGIN_FAILED);
        }
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}

void TcpClient::loadConfig() {                         // 初始化
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



#if 0
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
#endif

void TcpClient::on_login_pb_clicked() {                                // 点击登录
    QString strName = ui->name_le->text();                              // 读用户名
    QString strPwd = ui->pwd_le->text();                                // 读密码
    if (!strName.isEmpty() && !strPwd.isEmpty()) {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;                  // 消息类型：注册请求
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);        // 给前 32 位用户名
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);    // 后 32 位 密码
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);                   // 发送数据
        free(pdu);                                                      // 释放内存
        pdu = NULL;
    } else {
        QMessageBox::critical(this, "登录", "登录失败：用户名或者密码不能为空");
    }
}

void TcpClient::on_regist_pb_clicked() {                              // 点击注册
    QString strName = ui->name_le->text();                              // 读用户名
    QString strPwd = ui->pwd_le->text();                                // 读密码
    if (!strName.isEmpty() && !strPwd.isEmpty()) {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;                  // 消息类型：注册请求
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);        // 给前 32 位用户名
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);    // 后 32 位 密码
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);                   // 发送数据
        free(pdu);                                                      // 释放内存
        pdu = NULL;
    } else {
        QMessageBox::critical(this, "注册", "注册失败：用户名或者密码不能为空");
    }
}

void TcpClient::on_cancel_pb_clicked() {

}
