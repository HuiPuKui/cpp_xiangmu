#include "opedb.h"
#include <QMessageBox>
#include <QDebug>
#include <QString>

OpeDB::OpeDB(QObject *parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QSQLITE"); // 告知数据库是 Sqlite
}

OpeDB &OpeDB::getInstance() { // 单例模式
    static OpeDB instance;
    return instance;
}

void OpeDB::init() {
    m_db.setHostName("localhost");                                                     // 主机名
    m_db.setDatabaseName("C:\\git_dir\\cpp_xiangmu\\C++wangpan\\TcpServer\\cloud.db"); // 地址
    if (m_db.open()) {
        QSqlQuery query;
        query.exec("select * from usrInfo");                                           // 执行命令
        while (query.next()) {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    } else {                                                                           // 如果打不开就弹出打开数据库失败
        QMessageBox::critical(NULL, "打开数据库", "打开数据库失败");
    }
}

OpeDB::~OpeDB() { // 析构函数关闭数据库
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd) {
    if (NULL == name || NULL == pwd) { // 检验形参的有效性
        return false;
    }
    QString data = QString("insert into usrInfo(name, pwd) values(\'%1\', \'%2\')").arg(name).arg(pwd); // sql 语句
    QSqlQuery query;
    return query.exec(data);
}

bool OpeDB::handleLogin(const char *name, const char *pwd) {
    if (NULL == name || NULL == pwd) { // 检验形参的有效性
        return false;
    }
    QString data = QString("select * from usrInfo where name = \'%1\' and pwd = \'%2\' and online = 0;").arg(name).arg(pwd); // sql 语句
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if (query.next()) { // 有下一条数据代表查询到一个人，因为用户名唯一
        // 将状态改为在线
        data = QString("update usrInfo set online = 1 where name = \'%1\' and pwd = \'%2\' and online = 0;").arg(name).arg(pwd); // sql 语句
        qDebug() << data;
        QSqlQuery query;
        query.exec(data);
        return true;
    } else {
        return false;
    }
}
