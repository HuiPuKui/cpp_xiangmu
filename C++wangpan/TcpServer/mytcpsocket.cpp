#include "mytcpsocket.h"
#include "protocol.h"
#include <QDebug>
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket() {                                  // 当 socket 有数据过来了就会发出 readyRead 信号
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg())); // 用自己的 recvMsg 进行接收
    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));
}

QString MyTcpSocket::getName() {
    return m_strName;
}

void MyTcpSocket::recvMsg() {
//    qDebug() << this->bytesAvailable();
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
//        qDebug() << caName << ' ' << caPwd << ' ' << pdu->uiMsgType;
        bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);    // 把用户名、密码传给数据库，获得返回值
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if (ret == true) {
            strcpy(respdu->caData, REGIST_OK);                          // 成功就传成功
        } else {
            strcpy(respdu->caData, REGIST_FAILED);                      // 失败就传失败
        }
//        qDebug() << respdu->caData;
        write((char*)respdu, respdu->uiPDULen);
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
//        qDebug() << respdu->caData;
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);                                                   // 释放
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST: {
        QStringList ret = OpeDB::getInstance().handleAllOnline();       // 获得用户名列表
        uint uiMsgLen = ret.size() * 32;                                // 用个数推算占用的空间
        PDU *respdu = mkPDU(uiMsgLen);                                  // 申请内存
        respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        for (int i = 0; i < ret.size(); i ++) {                         // 一次将用户名拷贝进 caMsg
            memcpy((char*)(respdu->caMsg) + i * 32
                   , ret.at(i).toStdString().c_str()
                   , ret.at(i).size());
        }

        write((char*)respdu, respdu->uiPDULen);                            // 发送数据
        free(respdu);                                                   // 释放
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST: { // 看搜索的用户是否存在、在线、离线
        int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if (-1 == ret) {
            strcpy(respdu->caData, SEARCH_USR_NO);
        } else if (1 == ret) {
            strcpy(respdu->caData, SEARCH_USR_ONLINE);
        } else if (0 == ret) {
            strcpy(respdu->caData, SEARCH_USR_OFFLINE);
        }
        write((char*)respdu, respdu->uiPDULen);                            // 发送数据
        free(respdu);                                                   // 释放
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST : { // 加好友的请求
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName, pdu->caData, 32);
        strncpy(caName, pdu->caData + 32, 32);
        int ret = OpeDB::getInstance().handleAddFriend(caPerName, caName);
        PDU *respdu = NULL;
        if (-1 == ret) {                // 错误
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, UNKNOW_ERROR);
            write((char*)respdu, respdu->uiPDULen);                            // 发送数据
            free(respdu);                                                      // 释放
            respdu = NULL;
        } else if (0 == ret) {          // 已经是好友了
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, EXISTED_FIREND);
            write((char*)respdu, respdu->uiPDULen);                            // 发送数据
            free(respdu);                                                      // 释放
            respdu = NULL;
        } else if (1 == ret) {          // 存在
            MyTcpServer::getInstance().resend(caPerName, pdu);
        } else if (2 == ret) {          // 不在线
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
            write((char*)respdu, respdu->uiPDULen);                            // 发送数据
            free(respdu);                                                      // 释放
            respdu = NULL;
        } else if (3 == ret) {          // 不存在
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_NOEXIST);
//            qDebug() << respdu->caData;
            write((char*)respdu, respdu->uiPDULen);                            // 发送数据
            free(respdu);                                                      // 释放
            respdu = NULL;
        }

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE : {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        memcpy(caPerName, pdu->caData, 32);
        memcpy(caName, pdu->caData + 32, 32);
        OpeDB::getInstance().handleAgreeAddFriend(caPerName, caName);
        MyTcpServer::getInstance().resend(caName, pdu); // 向发起添加好友的那个人发送数据
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE : {
        char caName[32] = {'\0'};
        memcpy(caName, pdu->caData + 32, 32);
        MyTcpServer::getInstance().resend(caName, pdu); // 向发起添加好友的那个人发送数据
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST : {
        char caName[32] = {'\0'};
        memcpy(caName, pdu->caData, 32);
        QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);
        uint uiMsgLen = ret.size() * 32; // 列表中的名字一共所需要占用的空间
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        for (int i = 0; i < ret.size(); i ++) {
            memcpy((char*)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST : {
        char caSelfName[32] = {'\0'};
        char caFriendName[32] = {'\0'};
        strncpy(caSelfName, pdu->caData, 32);
        strncpy(caFriendName, pdu->caData + 32, 32);
        OpeDB::getInstance().handleDelFriend(caSelfName, caFriendName);

        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        strcpy(respdu->caData, DEL_FRIEND_OK);

        write((char*)respdu, respdu->uiPDULen);                 // 发送给删除人
        free(respdu);
        respdu = NULL;

        MyTcpServer::getInstance().resend(caFriendName, pdu);   // 发送提示给被删除人

        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST : {
        char caPerName[32] = {'\0'};
        memcpy(caPerName, pdu->caData + 32, 32);
//        qDebug() << caPerName;
        MyTcpServer::getInstance().resend(caPerName, pdu);
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
