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
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)), this, SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket) {
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for (;iter != m_tcpSocketList.end(); iter ++) {                     // 找到匹配的 mysocket 并删除
        if (mysocket == *iter) {
            (*iter)->deleteLater();                                     // 释放空间
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }
    for (int i = 0; i < m_tcpSocketList.size(); i ++) {                  // 输出目前还在的用户
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}
