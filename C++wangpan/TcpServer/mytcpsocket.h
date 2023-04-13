#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "opedb.h"
#include <QDir>

class MyTcpSocket : public QTcpSocket {
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
signals:
    void offline(MyTcpSocket *mysocket);
public slots:
    void recvMsg();
    void clientOffline();
private:
    QString m_strName;
};

#endif // MYTCPSOCKET_H
