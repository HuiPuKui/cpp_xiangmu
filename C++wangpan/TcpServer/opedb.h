#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject {
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();
    ~OpeDB();

    bool handleRegist(const char* name, const char* pwd);               // 注册
    bool handleLogin(const char* name, const char* pwd);                // 登录
    void handleOffline(const char* name);                               // 下线
    QStringList handleAllOnline();                                      // 在线用户
    int handleSearchUsr(const char *name);                              // 查找用户
    int handleAddFriend(const char* pername, const char* name);         // 添加好友
    void handleAgreeAddFriend(const char *pername, const char *name);   // 同意添加好友
    QStringList handleFlushFriend(const char *name);                    // 刷新
    bool handleDelFriend(const char *name, const char *friendName);     // 删除好友
signals:

public slots:
private:
    QSqlDatabase m_db;
};

#endif // OPEDB_H
