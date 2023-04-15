#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>

class Book : public QWidget {
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearEnterDir();
    QString enterDir();
    void setDownloadStatus(bool status);
    bool getDownloadStatus();
    QString getSaveFilePath();

    qint64 m_iTotal;        // 总的文件大小
    qint64 m_iRecved;       // 已经收到的文件大小

signals:

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void renameFile();
    void enterDir(const QModelIndex &index);
    void returnPre();
    void delRegFile();
    void uploadFile();

    void uploadFileData();

    void downloadFile();

    void shareFile();

private:
    QListWidget *m_pBookListW;      // 文件夹列表
    QPushButton *m_pReturnPB;       // 返回
    QPushButton *m_pCreateDirPB;    // 创建文件夹
    QPushButton *m_pDelDirPB;       // 删除文件夹
    QPushButton *m_pRenamePB;       // 重命名文件
    QPushButton *m_pFlushFilePB;    // 刷新文件
    QPushButton *m_pUploadPB;       // 上传文件
    QPushButton *m_pDownLoadPB;     // 下载文件
    QPushButton *m_pDelFilePB;      // 删除文件
    QPushButton *m_pShareFilePB;    // 分享文件

    QString m_strEnterDir;          // 当前文件名
    QString m_strUploadFilePath;    // 打开的那个路径

    QTimer *m_pTimer;               // 定时器
    QString m_strSaveFilePath;      // 保存路径
    bool m_bDownload;               // 是否处于下载状态
};

#endif // BOOK_H
