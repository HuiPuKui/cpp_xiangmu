#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed : name error or pwd error or relogin"

enum ENUM_MSG_TYPE {
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,   // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   // 注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,   // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,   // 登录回复
//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,
//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,
//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,
//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,
    ENUM_MSG_TYPE_MAX = 0x00ffffff
};

struct PDU {            // 协议数据单元
    uint uiPDULen;      // 总的协议数据单元大小
    uint uiMsgType;     // 消息类型
    char caData[64];
    uint uiMsgLen;      // 实际消息长度
    int caMsg[];        // 实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
