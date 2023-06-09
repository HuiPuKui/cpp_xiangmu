#include "sharefile.h"
#include <QDebug>
#include "tcpclient.h"
#include "opewidget.h"
#include "protocol.h"

ShareFile::ShareFile(QWidget *parent) : QWidget(parent) {
    m_pSelectAllPB = new QPushButton("全选");         // 全选
    m_pCancelSelectPB = new QPushButton("取消选择");   // 取消全选

    m_pOKPB = new QPushButton("确定");                // 确定
    m_pCancelPB = new QPushButton("取消");            // 取消

    m_pSA = new QScrollArea;                         // 展示区域
    m_pFriendW = new QWidget;
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup = new QButtonGroup;               // 管理选项
    m_pButtonGroup->setExclusive(false);             // false: 多选   true: 单选

    // 布局
    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);
    setLayout(pMainVBL);

    connect(m_pCancelSelectPB, SIGNAL(clicked(bool)), this, SLOT(cancelSelect()));
    connect(m_pSelectAllPB, SIGNAL(clicked(bool)), this, SLOT(selectAll()));
    connect(m_pOKPB, SIGNAL(clicked(bool)), this, SLOT(okShare()));
    connect(m_pCancelPB, SIGNAL(clicked(bool)), this, SLOT(cancelShare()));
}

ShareFile &ShareFile::getInstance() {
    static ShareFile instance;
    return instance;
}

void ShareFile::test() {
    QVBoxLayout *p = new QVBoxLayout(m_pFriendW);
    QCheckBox *pCB = NULL;
    for (int i = 0; i < 15; i ++) {
        pCB = new QCheckBox("jack");
        p->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::updateFriend(QListWidget *pFriendList) {
    if (NULL == pFriendList) {
        return ;
    }
    QAbstractButton *tmp = NULL;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();

//    qDebug() << preFriendList.size();

    for (int i = 0; i < preFriendList.size(); i ++) {
        tmp = preFriendList[i];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        preFriendList.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }

//    qDebug() << pFriendList->count();

    QCheckBox *pCB = NULL;
    for (int i = 0; i < pFriendList->count(); i ++) {
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);

        pCB = NULL;
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::cancelSelect() {
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for (int i = 0; i < cbList.size(); i ++) {
        if (cbList[i]->isChecked()) {
            cbList[i]->setChecked(false);
        }
    }
}

void ShareFile::selectAll() {
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for (int i = 0; i < cbList.size(); i ++) {
        if (!cbList[i]->isChecked()) {
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::okShare() {
    QString strName = TcpClient::getInstance().loginName();
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();

    QString strPath = strCurPath + "/" + strShareFileName;

    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0;
    for (int i = 0; i < cbList.size(); i ++) {
        if (cbList[i]->isChecked()) {
            num ++;
        }
    }

    PDU *pdu = mkPDU(32 * num + strPath.size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData, "%s %d", strName.toStdString().c_str(), num);
    int j = 0;
    for (int i = 0; i < cbList.size(); i ++) {
        if (cbList[i]->isChecked()) {
            memcpy((char*)(pdu->caMsg) + j * 32, cbList[i]->text().toStdString().c_str(), cbList[i]->text().size());
            j ++;
        }
    }

    memcpy((char*)(pdu->caMsg) + num * 32, strPath.toStdString().c_str(), strPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void ShareFile::cancelShare() {
    hide();
}
