#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer() {

}

MyTcpServer &MyTcpServer::getInstance() {
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor) {
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;          // 创建一个新的 MyTcpSocket

    pTcpSocket->setSocketDescriptor(socketDescriptor);  // 用这个来接收文件描述符
    m_tcpSocketList.append(pTcpSocket);                 // 放入列表中
}
