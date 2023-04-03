
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>



QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget

{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

private:
    Ui::TcpClient *ui;
};

#endif // TCPCLIENT_H
