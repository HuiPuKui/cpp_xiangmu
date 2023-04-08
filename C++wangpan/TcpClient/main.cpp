#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

//    TcpClient w;
//    w.show();            // 将 w 对象设为可见

    TcpClient::getInstance().show();

    return a.exec();     // 进入事件循环
}
