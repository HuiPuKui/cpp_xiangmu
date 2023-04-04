#include "protocol.h"

PDU *mkPDU(uint uiMsgLen) {                 // 协议动态空间申请
    uint uiPDULen = sizeof(PDU) + uiMsgLen; // 计算总的协议数据单元大小
    PDU *pdu = (PDU*)malloc(uiPDULen);      // 申请空间
    if (NULL == pdu) {                      // 如果空间申请失败则停止程序
        exit(EXIT_FAILURE);
    }
    memset(pdu, 0, uiPDULen);               // 清空 pdu
    pdu->uiPDULen = uiPDULen;               // 复赋值
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
