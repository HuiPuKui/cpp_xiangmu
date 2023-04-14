#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "opewidget.h"
#include "privatechat.h"

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
//    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));                        // 首先读 sizeof(uint) 大小的字节，将 uiPDULen 读进来
    uint uiMsgLen = uiPDULen - sizeof(PDU);                                  // 那么实际消息长度就是总的协议数据单元大小减去 sizeof(PDU)
    PDU *pdu = mkPDU(uiMsgLen);                                              // 因此在弹性结构体申请 uiMsgLen 大小的空间进行接收
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));    // 接收要在上个接受的之后，所以加一个偏移量
//    qDebug() << pdu->uiMsgType;
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
            m_strCurPath = QString("./%1").arg(m_strLoginName);
            QMessageBox::information(this, "登录", LOGIN_OK);
            OpeWidget::getInstance().show();                                // 显示跳转的窗口
            this->hide();                                                   // 隐藏登录界面
        } else if (0 == strcmp(pdu->caData, LOGIN_FAILED)) {                // 登录失败
            QMessageBox::warning(this, "登录", LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND: {
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);        // 把得到的 pdu 一层一层传下去
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND : { // 对应搜索不存在、在线、离线的情况
        if (0 == strcmp(SEARCH_USR_NO, pdu->caData)) {
            QMessageBox::information(this, "搜索", QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        } else if (0 == strcmp(SEARCH_USR_ONLINE, pdu->caData)) {
            QMessageBox::information(this, "搜索", QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        } else if (0 == strcmp(SEARCH_USR_OFFLINE, pdu->caData)) {
            QMessageBox::information(this, "搜索", QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST : {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData + 32, 32);
        int ret = QMessageBox::information(this, "添加好友", QString("%1 want to add you as friend ?").arg(caName), QMessageBox::Yes, QMessageBox::No); // 弹出想要加好友的框
        PDU *respdu = mkPDU(0);
        memcpy(respdu->caData, pdu->caData, 64);                 // 放入自己的用户名和对方用户名
        if (QMessageBox::Yes == ret) {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE; // 同意
        } else {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE; // 不同意
        }
        m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND: {
        QMessageBox::information(this, "添加好友", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE : {
        char caPerName[32] = {'\0'};
        memcpy(caPerName, pdu->caData, 32);
        QMessageBox::information(this, "添加好友", QString("添加%1好友成功").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE : {
        char caPerName[32] = {'\0'};
        memcpy(caPerName, pdu->caData, 32);
        QMessageBox::information(this, "添加好友", QString("添加%2好友失败").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND : {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST : {
        char caName[32] = {'\0'};
        memcpy(caName, pdu->caData, 32);
        QMessageBox::information(this, "删除好友", QString("%1 删除你作为他的好友").arg(caName));
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND : {
        QMessageBox::information(this, "删除好友", "删除好友成功");
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST : {
        if (PrivateChat::getInstance().isHidden()) {
            PrivateChat::getInstance().show();
        }
        char caSendName[32] = {'\0'};
        memcpy(caSendName, pdu->caData, 32);
        QString strSendName = caSendName;
        PrivateChat::getInstance().setChatName(strSendName);
        PrivateChat::getInstance().updateMsg(pdu);

        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST : {
        OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND : {
        QMessageBox::information(this, "创建文件", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND : {
        OpeWidget::getInstance().getBook()->updateFileList(pdu);
        QString strEnterDir = OpeWidget::getInstance().getBook()->enterDir();
        if (!strEnterDir.isEmpty()) {
            m_strCurPath = m_strCurPath + "/" + strEnterDir;
            qDebug() << m_strCurPath;
        }
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND : {
        QMessageBox::information(this, "删除文件夹", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND : {
        QMessageBox::information(this, "重命名文件", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ENTER_DIR_RESPOND : {
        OpeWidget::getInstance().getBook()->clearEnterDir();
        QMessageBox::information(this, "进入文件夹", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND : {
        QMessageBox::information(this, "上传文件", pdu->caData);
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

TcpClient &TcpClient::getInstance() {
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket() {
    return m_tcpSocket;
}

QString TcpClient::loginName() {
    return m_strLoginName;
}

QString TcpClient::curPath() {
    return m_strCurPath;
}

void TcpClient::setCurPath(QString strCurPath) {
    m_strCurPath = strCurPath;
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
        m_strLoginName = strName;
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
