#include "mytcpsocket.h"
#include "protocol.h"
#include <QDebug>

MyTcpSocket::MyTcpSocket() {                                  // 当 socket 有数据过来了就会发出 readyRead 信号
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg())); // 用自己的 recvMsg 进行接收
    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));
}

QString MyTcpSocket::getName() {
    return m_strName;
}

void MyTcpSocket::recvMsg() {
    qDebug() << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));                     // 首先读 sizeof(uint) 大小的字节，将 uiPDULen 读进来
    uint uiMsgLen = uiPDULen - sizeof(PDU);                         // 那么实际消息长度就是总的协议数据单元大小减去 sizeof(PDU)
    PDU *pdu = mkPDU(uiMsgLen);                                     // 因此在弹性结构体申请 uiMsgLen 大小的空间进行接收
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 接收要在上个接受的之后，所以加一个偏移量
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGIST_REQUEST: {
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);                               // 提取用户名
        strncpy(caPwd, pdu->caData + 32, 32);                           // 提取密码
        qDebug() << caName << ' ' << caPwd << ' ' << pdu->uiMsgType;
        bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);    // 把用户名、密码传给数据库，获得返回值
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if (ret == true) {
            strcpy(respdu->caData, REGIST_OK);                          // 成功就传成功
        } else {
            strcpy(respdu->caData, REGIST_FAILED);                      // 失败就传失败
        }
        qDebug() << respdu->caData;
        write((char*)respdu, pdu->uiPDULen);
        free(respdu);                                                   // 释放
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST: {
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);                               // 提取用户名
        strncpy(caPwd, pdu->caData + 32, 32);                           // 提取密码
        bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);     // 把用户名、密码传给数据库，获得返回值
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
        if (ret == true) {
            strcpy(respdu->caData, LOGIN_OK);                          // 成功就传成功
            m_strName = caName;
        } else {
            strcpy(respdu->caData, LOGIN_FAILED);                      // 失败就传失败
        }
        qDebug() << respdu->caData;
        write((char*)respdu, pdu->uiPDULen);
        free(respdu);                                                   // 释放
        respdu = NULL;
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;

//    qDebug() << caName << ' ' << caPwd << ' ' << pdu->uiMsgType;

}

void MyTcpSocket::clientOffline() {
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}
