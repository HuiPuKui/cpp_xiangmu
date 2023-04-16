#include "mytcpsocket.h"
#include "protocol.h"
#include <QDebug>
#include "mytcpserver.h"
#include <QDir>

MyTcpSocket::MyTcpSocket() {                                  // 当 socket 有数据过来了就会发出 readyRead 信号
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg())); // 用自己的 recvMsg 进行接收
    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));
    m_bUpload = false;
    m_pTimer = new QTimer;
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(sendFileToClient()));
}

QString MyTcpSocket::getName() {
    return m_strName;
}

void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir) { // 递归拷贝
    QDir dir;
    dir.mkdir(strDestDir);

    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();

    QString srcTmp, destTmp;
    for (int i = 0; i < fileInfoList.size(); i ++) {
        qDebug() << fileInfoList[i].fileName();
        if (fileInfoList[i].isFile()) {
            srcTmp = strSrcDir + "/" + fileInfoList[i].fileName();
            destTmp = strDestDir + "/" + fileInfoList[i].fileName();
            QFile::copy(srcTmp, destTmp);
        } else if (fileInfoList[i].isDir()) {
            if (QString(".") == fileInfoList[i].fileName() || QString("..") == fileInfoList[i].fileName()) {
                continue;
            }
            srcTmp = strSrcDir + "/" + fileInfoList[i].fileName();
            destTmp = strDestDir + "/" + fileInfoList[i].fileName();
            copyDir(srcTmp, destTmp);
        }
    }
}

void MyTcpSocket::recvMsg() {
    if (!m_bUpload) {
//        qDebug() << this->bytesAvailable();
        uint uiPDULen = 0;
        this->read((char*)&uiPDULen, sizeof(uint));                     // 首先读 sizeof(uint) 大小的字节，将 uiPDULen 读进来
        uint uiMsgLen = uiPDULen - sizeof(PDU);                         // 那么实际消息长度就是总的协议数据单元大小减去 sizeof(PDU)
        PDU *pdu = mkPDU(uiMsgLen);                                     // 因此在弹性结构体申请 uiMsgLen 大小的空间进行接收
        this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 接收要在上个接受的之后，所以加一个偏移量
        switch (pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGIST_REQUEST: {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);                               // 提取用户名
            strncpy(caPwd, pdu->caData + 32, 32);                           // 提取密码
//            qDebug() << caName << ' ' << caPwd << ' ' << pdu->uiMsgType;
            bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);    // 把用户名、密码传给数据库，获得返回值
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if (ret == true) {
                strcpy(respdu->caData, REGIST_OK);                          // 成功就传成功
                QDir dir;
                qDebug() << dir.mkdir(QString("./%1").arg(caName));                     // 创建目录

            } else {
                strcpy(respdu->caData, REGIST_FAILED);                      // 失败就传失败
            }
//            qDebug() << respdu->caData;
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);                                                   // 释放
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST: {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);                               // 提取用户名
            strncpy(caPwd, pdu->caData + 32, 32);                           // 提取密码
            bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);     // 把用户名、密码传给数据库，获得返回值
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if (ret == true) {
                strcpy(respdu->caData, LOGIN_OK);                          // 成功就传成功
                m_strName = caName;
            } else {
                strcpy(respdu->caData, LOGIN_FAILED);                      // 失败就传失败
            }
//            qDebug() << respdu->caData;
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);                                                   // 释放
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST: {
            QStringList ret = OpeDB::getInstance().handleAllOnline();       // 获得用户名列表
            uint uiMsgLen = ret.size() * 32;                                // 用个数推算占用的空间
            PDU *respdu = mkPDU(uiMsgLen);                                  // 申请内存
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for (int i = 0; i < ret.size(); i ++) {                         // 一次将用户名拷贝进 caMsg
                memcpy((char*)(respdu->caMsg) + i * 32
                       , ret.at(i).toStdString().c_str()
                       , ret.at(i).size());
            }

            write((char*)respdu, respdu->uiPDULen);                            // 发送数据
            free(respdu);                                                   // 释放
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_REQUEST: { // 看搜索的用户是否存在、在线、离线
            int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
            if (-1 == ret) {
                strcpy(respdu->caData, SEARCH_USR_NO);
            } else if (1 == ret) {
                strcpy(respdu->caData, SEARCH_USR_ONLINE);
            } else if (0 == ret) {
                strcpy(respdu->caData, SEARCH_USR_OFFLINE);
            }
            write((char*)respdu, respdu->uiPDULen);                            // 发送数据
            free(respdu);                                                   // 释放
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST : { // 加好友的请求
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName, pdu->caData, 32);
            strncpy(caName, pdu->caData + 32, 32);
            int ret = OpeDB::getInstance().handleAddFriend(caPerName, caName);
            PDU *respdu = NULL;
            if (-1 == ret) {                // 错误
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, UNKNOW_ERROR);
                write((char*)respdu, respdu->uiPDULen);                            // 发送数据
                free(respdu);                                                      // 释放
                respdu = NULL;
            } else if (0 == ret) {          // 已经是好友了
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, EXISTED_FIREND);
                write((char*)respdu, respdu->uiPDULen);                            // 发送数据
                free(respdu);                                                      // 释放
                respdu = NULL;
            } else if (1 == ret) {          // 存在
                MyTcpServer::getInstance().resend(caPerName, pdu);
            } else if (2 == ret) {          // 不在线
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
                write((char*)respdu, respdu->uiPDULen);                            // 发送数据
                free(respdu);                                                      // 释放
                respdu = NULL;
            } else if (3 == ret) {          // 不存在
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_NOEXIST);
//                qDebug() << respdu->caData;
                write((char*)respdu, respdu->uiPDULen);                            // 发送数据
                free(respdu);                                                      // 释放
                respdu = NULL;
            }

            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE : {
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            memcpy(caPerName, pdu->caData, 32);
            memcpy(caName, pdu->caData + 32, 32);
            OpeDB::getInstance().handleAgreeAddFriend(caPerName, caName);
            MyTcpServer::getInstance().resend(caName, pdu); // 向发起添加好友的那个人发送数据
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE : {
            char caName[32] = {'\0'};
            memcpy(caName, pdu->caData + 32, 32);
            MyTcpServer::getInstance().resend(caName, pdu); // 向发起添加好友的那个人发送数据
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST : {
            char caName[32] = {'\0'};
            memcpy(caName, pdu->caData, 32);
            QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);
            uint uiMsgLen = ret.size() * 32; // 列表中的名字一共所需要占用的空间
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for (int i = 0; i < ret.size(); i ++) {
                memcpy((char*)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST : {
            char caSelfName[32] = {'\0'};
            char caFriendName[32] = {'\0'};
            strncpy(caSelfName, pdu->caData, 32);
            strncpy(caFriendName, pdu->caData + 32, 32);
            OpeDB::getInstance().handleDelFriend(caSelfName, caFriendName);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            strcpy(respdu->caData, DEL_FRIEND_OK);

            write((char*)respdu, respdu->uiPDULen);                 // 发送给删除人
            free(respdu);
            respdu = NULL;

            MyTcpServer::getInstance().resend(caFriendName, pdu);   // 发送提示给被删除人

            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST : {
            char caPerName[32] = {'\0'};
            memcpy(caPerName, pdu->caData + 32, 32);
//            qDebug() << caPerName;
            MyTcpServer::getInstance().resend(caPerName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST : {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName); // 得到 caName 的所有在线好友
            QString tmp;
            for (int i = 0; i < onlineFriend.size(); i ++) {
                tmp = onlineFriend.at(i);
                MyTcpServer::getInstance().resend(tmp.toStdString().c_str(), pdu);
            }
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST : {
            QDir dir;
            QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
            qDebug() << strCurPath;
            bool ret = dir.exists(QString(strCurPath)); // 判断路径是否存在
            PDU *respdu = NULL;
            if (ret) {
                char caNewDir[32] = {'\0'};
                memcpy(caNewDir, pdu->caData + 32, 32);
                QString strNewPath = strCurPath + "/" + caNewDir; // 新路径
//                qDebug() << strNewPath;
                ret = dir.exists(strNewPath);
//                qDebug() << "->" << ret;
                if (ret) { // 创建的文件名已经存在
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData, FILE_NAME_EXIST);
                } else {
                    dir.mkdir(strNewPath);
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData, CREAT_DIR_OK);
                }
            } else {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData, DIR_NO_EXIST);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST : {
            char *pCurPath = new char[pdu->uiMsgLen];
            memcpy(pCurPath, pdu->caMsg, pdu->uiMsgLen);
            QDir dir(pCurPath);
            QFileInfoList fileInfoList = dir.entryInfoList(); // 获取文件列表
            int iFileCount = fileInfoList.size();
            PDU *respdu = mkPDU(sizeof(FileInfo) * iFileCount);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            FileInfo *pFileInfo = NULL;
            QString strFileName;
            for (int i = 0; i < iFileCount; i ++) {
                pFileInfo = (FileInfo*)(respdu->caMsg) + i; // 偏移量
                strFileName = fileInfoList[i].fileName();
                memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());
                if (fileInfoList[i].isDir()) {
                    pFileInfo->iFileType = 0;
                } else if (fileInfoList[i].isFile()) {
                    pFileInfo->iFileType = 1;
                }
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_REQUEST : {
            char caName[32] = {'\0'};
            strcpy(caName, pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caName); // 拼接要删除的路径
//            qDebug() << strPath;
            QFileInfo fileInfo(strPath);
            bool ret = false;
            if (fileInfo.isDir()) { // 文件夹
                QDir dir;
                dir.setPath(strPath);
                ret = dir.removeRecursively();
            } else if (fileInfo.isFile()) { // 常规文件
                ret = false;
            }
            PDU *respdu = NULL;
            if (ret) {
                respdu = mkPDU(strlen(DEL_DIR_OK) + 1);
                memcpy(respdu->caData, DEL_DIR_OK, strlen(DEL_DIR_OK));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;

            } else {
                respdu = mkPDU(strlen(DEL_DIR_FAILURED) + 1);
                memcpy(respdu->caData, DEL_DIR_FAILURED, strlen(DEL_DIR_FAILURED));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST : {
            char caOldName[32] = {'\0'};
            char caNewName[32] = {'\0'};
            strncpy(caOldName, pdu->caData, 32);
            strncpy(caNewName, pdu->caData + 32, 32);

            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);

            QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldName);
            QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewName);

            qDebug() << strOldPath;
            qDebug() << strNewPath;

            QDir dir;
            bool ret = dir.rename(strOldPath, strNewPath);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            if (ret) {
                strcpy(respdu->caData, RENAME_FILE_OK);
            } else {
                strcpy(respdu->caData, RENAME_FILE_FAILURED);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST : {
            char caEnterName[32] = {'\0'};
            strncpy(caEnterName, pdu->caData, 32);

            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);

            QString strPath = QString("%1/%2").arg(pPath).arg(caEnterName);

            qDebug() << strPath;

            QFileInfo fileInfo(strPath);
            PDU *respdu = NULL;
            if (fileInfo.isDir()) {
                QDir dir(strPath);
                QFileInfoList fileInfoList = dir.entryInfoList(); // 获取文件列表
                int iFileCount = fileInfoList.size();
                respdu = mkPDU(sizeof(FileInfo) * iFileCount);
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
                FileInfo *pFileInfo = NULL;
                QString strFileName;
                for (int i = 0; i < iFileCount; i ++) {
                    pFileInfo = (FileInfo*)(respdu->caMsg) + i; // 偏移量
                    strFileName = fileInfoList[i].fileName();
                    memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());
                    if (fileInfoList[i].isDir()) {
                        pFileInfo->iFileType = 0;
                    } else if (fileInfoList[i].isFile()) {
                        pFileInfo->iFileType = 1;
                    }
                }
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            } else if (fileInfo.isFile()) {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(respdu->caData, ENTER_DIR_FAILURED);

                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST : {
            char caFileName[32] = {'\0'};
            qint64 fileSize = 0;
            sscanf(pdu->caData, "%s %lld", caFileName, &fileSize);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug() << strPath;

            delete [] pPath;
            pPath = NULL;

            m_file.setFileName(strPath);
            if (m_file.open(QIODevice::WriteOnly)) { // 以只写的方式打开文件，若文件不存在，则会自动创建文件
                m_bUpload = true;
                m_iTotal= fileSize;
                m_iRecved = 0;
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_REQUEST : {
            char caName[32] = {'\0'};
            strcpy(caName, pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caName); // 拼接要删除的路径
//            qDebug() << strPath;
            QFileInfo fileInfo(strPath);
            bool ret = false;
            if (fileInfo.isDir()) { // 文件夹
                ret = false;
            } else if (fileInfo.isFile()) { // 常规文件
                QDir dir;
                ret = dir.remove(strPath);
            }
            PDU *respdu = NULL;
            if (ret) {
                respdu = mkPDU(strlen(DEL_FILE_OK) + 1);
                memcpy(respdu->caData, DEL_FILE_OK, strlen(DEL_FILE_OK));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;

            } else {
                respdu = mkPDU(strlen(DEL_FILE_FAILURED) + 1);
                memcpy(respdu->caData, DEL_FILE_FAILURED, strlen(DEL_FILE_FAILURED));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST : {
            char caFileName[32] = {'\0'};
            strcpy(caFileName, pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug() << strPath;
            delete [] pPath;
            pPath = NULL;

            QFileInfo fileInfo(strPath);
            qint64 fileSize = fileInfo.size();
            PDU *respdu = mkPDU(0);
            sprintf(respdu->caData, "%s %lld", caFileName, fileSize);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;

            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            m_file.setFileName(strPath);
            m_file.open(QIODevice::ReadOnly);
            m_pTimer->start(1000);

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST : {
            char caSendName[32] = {'\0'};
            int num = 0;

            sscanf(pdu->caData, "%s%d", caSendName, &num);
            int size = num * 32; // 接收者所占大小

            PDU *respdu = mkPDU(pdu->uiMsgLen - size);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            strcpy(respdu->caData, caSendName);
            memcpy(respdu->caMsg, (char*)(pdu->caMsg) + size, pdu->uiMsgLen - size);

            char caRecvName[32] = {'\0'};
            for (int i = 0; i < num; i ++) {
                memcpy(caRecvName, (char*)(pdu->caMsg) + i * 32, 32);
                qDebug() << caRecvName;
                MyTcpServer::getInstance().resend(caRecvName, respdu);
            }

            free(respdu);
            respdu = NULL;

            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData, "share file ok");
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND : {
            QString strRecvPath = QString("./%1").arg(pdu->caData);
            QString strShareFilePath = QString("%1").arg((char*)(pdu->caMsg));
            int index = strShareFilePath.lastIndexOf('/');
            QString strFileName = strShareFilePath.right(strShareFilePath.size() - index - 1);
            strRecvPath = strRecvPath + "/" + strFileName;

            QFileInfo fileInfo(strShareFilePath);
            if (fileInfo.isFile()) {
                QFile::copy(strShareFilePath, strRecvPath);
            } else if (fileInfo.isDir()) {
                copyDir(strShareFilePath, strRecvPath);
            }
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST : {
            char caFileName[32] = {'\0'};
            int srcLen = 0, destLen = 0;
            sscanf(pdu->caData, "%d%d%s", &srcLen, &destLen, caFileName);

            char *pSrcPath = new char[srcLen + 1];
            char *pDestPath = new char[destLen + 1 + 32]; // 加 32 因为要拼接名字
            memset(pSrcPath, '\0', srcLen + 1);
            memset(pDestPath, '\0', destLen + 1 + 32);

            memcpy(pSrcPath, pdu->caMsg, srcLen);
            memcpy(pDestPath, (char*)(pdu->caMsg) + (srcLen + 1), destLen);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
            QFileInfo fileInfo(pDestPath);
            if (fileInfo.isDir()) {
                strcat(pDestPath, "/");
                strcat(pDestPath, caFileName);
                qDebug() << pDestPath;
                bool ret = QFile::rename(pSrcPath, pDestPath);
                if (ret) {
                    strcpy(respdu->caData, MOVE_FILE_OK);
                } else {
                    strcpy(respdu->caData, COMMON_ERR);
                }
            } else if (fileInfo.isFile()) {
                strcpy(respdu->caData, MOVE_FILE_FAILURED);
            }
            qDebug() << respdu->caData;
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
    } else { // 直接以二进制形式传过来，不以 PDU 形式
        PDU *respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;

        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        if (m_iTotal == m_iRecved) { // 如果 <= 代表接收结束了
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData, UPLOAD_FILE_OK);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        } else if (m_iTotal < m_iRecved) {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData, UPLOAD_FILE_FAILURED);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
    }

//    qDebug() << caName << ' ' << caPwd << ' ' << pdu->uiMsgType;

}

void MyTcpSocket::clientOffline() {
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}

void MyTcpSocket::sendFileToClient() {
    char *pData = new char[4096];
    qint64 ret = 0;
    while (true) {
        ret = m_file.read(pData, 4096);
        if (ret > 0 && ret <= 4096) {
            write(pData, ret);
        } else if (0 == ret) {
            m_file.close();
            break;
        } else if (ret < 0) {
            qDebug() << "发送文件内容给客户端过程中失败";
            m_file.close();
            break;
        }
    }
    delete [] pData;
    pData = NULL;
}
