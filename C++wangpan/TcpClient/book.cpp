#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>

/*
 *
 * 图书界面设计
 *
 */

Book::Book(QWidget *parent) : QWidget(parent) {
    m_pBookListW = new QListWidget;                  // 文件夹列表
    m_pReturnPB = new QPushButton("返回");            // 返回
    m_pCreateDirPB = new QPushButton("创建文件夹");    // 创建文件夹
    m_pDelDirPB = new QPushButton("删除文件夹");       // 删除文件夹
    m_pRenamePB = new QPushButton("重命名文件");       // 重命名文件
    m_pFlushFilePB = new QPushButton("刷新文件");      // 刷新文件

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadPB = new QPushButton("上传文件");        // 上传文件
    m_pDownLoadPB = new QPushButton("下载文件");      // 下载文件
    m_pDelFilePB = new QPushButton("删除文件");       // 删除文件
    m_pShareFilePB = new QPushButton("分享文件");     // 分享文件

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(createDir()));
    connect(m_pFlushFilePB, SIGNAL(clicked(bool)), this, SLOT(flushFile()));
}

void Book::updateFileList(const PDU *pdu) {
    if (NULL == pdu) {
        return ;
    }
    FileInfo *pFileInfo = NULL;
    int icount = pdu->uiMsgLen / sizeof(FileInfo);
    for (int i = 0; i < icount; i ++) {
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;
        qDebug() << pFileInfo->caFileName
                 << pFileInfo->iFileType;
    }
}

void Book::createDir() {
    QString strNewDir = QInputDialog::getText(this, "新建文件夹", "新文件夹名字"); // 新文件夹名
    if (!strNewDir.isEmpty()) {
        if (strNewDir.size() > 32) {
            QMessageBox::warning(this, "新建文件夹", "文件夹名字长度不能超过 32 个字符");
        } else {
            QString strName = TcpClient::getInstance().loginName(); // 用户名
            QString strCurPath = TcpClient::getInstance().curPath(); // 目录

            PDU *pdu = mkPDU(strCurPath.size() + 1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData, strName.toStdString().c_str(), strName.size());
            strncpy(pdu->caData + 32, strNewDir.toStdString().c_str(), strNewDir.size());
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }

    } else {
        QMessageBox::warning(this, "新建文件夹", "新文件夹名字不能为空");
    }
}

void Book::flushFile() {
    QString strCurPath = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg), strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}
