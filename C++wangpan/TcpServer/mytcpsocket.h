#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "opedb.h"
#include <QDir>
#include <QFile>

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

    QFile m_file;
    qint64 m_iTotal; // 总共大小
    qint64 m_iRecved; // 接收的大小
    bool m_bUpload; // 处于的状态

};

#endif // MYTCPSOCKET_H
