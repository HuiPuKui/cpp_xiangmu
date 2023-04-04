#include "mytcpsocket.h"
#include "protocol.h"
#include <QDebug>

MyTcpSocket::MyTcpSocket() {                                  // 当 socket 有数据过来了就会发出 readyRead 信号
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg())); // 用自己的 recvMsg 进行接收
}

void MyTcpSocket::recvMsg() {
    qDebug() << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));                     // 首先读 sizeof(uint) 大小的字节，将 uiPDULen 读进来
    uint uiMsgLen = uiPDULen - sizeof(PDU);                         // 那么实际消息长度就是总的协议数据单元大小减去 sizeof(PDU)
    PDU *pdu = mkPDU(uiMsgLen);                                     // 因此在弹性结构体申请 uiMsgLen 大小的空间进行接收
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 接收要在上个接受的之后，所以加一个偏移量
    qDebug() << pdu->uiMsgType << ' ' << (char*)(pdu->caMsg);
}
