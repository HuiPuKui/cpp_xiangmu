#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"

class Book : public QWidget {
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);

signals:

public slots:
    void createDir();
    void flushFile();

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
};

#endif // BOOK_H
