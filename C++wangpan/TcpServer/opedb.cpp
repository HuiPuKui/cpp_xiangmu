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

bool OpeDB::handleLogin(const char *name, const char *pwd) { // 处理上线
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

void OpeDB::handleOffline(const char *name) { // 处理下线
    if (NULL == name) {
        qDebug() << "name is NULL";
        return;
    }
    QString data = QString("update usrInfo set online = 0 where name = \'%1\';").arg(name);

    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleAllOnline() { // 查询在线人数
    QString data = QString("select name from usrInfo where online = 1;");

    QSqlQuery query;
    query.exec(data);

    QStringList result; // 存放结果集
    result.clear();

    while (query.next()) { // 逐条读取用户名
        result.append(query.value(0).toString());
    }
    return result;
}

/*
 *
 * 返回值 含义
 *  -1   不存在
 *   0   不在线
 *   1    在线
 *
 */

int OpeDB::handleSearchUsr(const char *name) { // 查询名为 name 的用户
    if (NULL == name) {
        return -1;
    }
    QString data = QString("select online from usrInfo where name = \'%1\';").arg(name);
    QSqlQuery query;
    query.exec(data);
    if (query.next()) {
        int ret = query.value(0).toInt();
        if (ret == 1) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}

int OpeDB::handleAddFriend(const char *pername, const char *name) {
    if (NULL == pername || NULL == name) {
        return -1;
    }
    QString data = QString("select * from friend where (id = (select id from usrInfo where name = \'%1\') and friendId = (select id from usrInfo where name = \'%2\')) "
                           "or (friendId = (select id from usrInfo where name = \'%3\') and id = (select id from usrInfo where name = \'%4\'));").arg(pername).arg(name).arg(name).arg(pername);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);

    if (query.next()) { // 双方已是好友
        return 0;
    } else {            // 不是好友
        data = QString("select online from usrInfo where name = \'%1\';").arg(pername);
        QSqlQuery query;
        query.exec(data);
        if (query.next()) {
            int ret = query.value(0).toInt();
            qDebug() << ret;
            if (ret == 1) {
                return 1; // 在线
            } else {
                return 2; // 不在线
            }
        } else {
            qDebug() << 3;
            return 3;     // 用户名不存在
        }
    }
}

void OpeDB::handleAgreeAddFriend(const char *pername, const char *name) {
    if (NULL == pername || NULL == name) {
        return ;
    }
    QString data = QString("insert into friend(id, friendId) values((select id from usrInfo where name = \'%1\'),(select id from usrInfo where name = \'%2\'));").arg(pername).arg(name);
    QSqlQuery query;
    query.exec(data);
}
