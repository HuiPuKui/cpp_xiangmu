#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "opedb.h"
#include <QDir>
#include <QFile>
#include <QTimer>

class MyTcpSocket : public QTcpSocket {
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
    void copyDir(QString strSrcDir, QString strDestDir);
signals:
    void offline(MyTcpSocket *mysocket);
public slots:
    void recvMsg();
    void clientOffline();
    void sendFileToClient();
private:
    QString m_strName;

    QFile m_file;
    qint64 m_iTotal; // 总共大小
    qint64 m_iRecved; // 接收的大小
    bool m_bUpload; // 处于的状态

    QTimer *m_pTimer; // 定时器
};

#endif // MYTCPSOCKET_H
