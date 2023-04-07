#include "opewidget.h"

/*
 *
 * 主要界面
 *
 */

OpeWidget::OpeWidget(QWidget *parent) : QWidget(parent) {
    m_pListW = new QListWidget(this);
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");

    m_pFriend = new Friend;
    m_pBook = new Book;

    m_pSW = new QStackedWidget;      // 堆栈窗口：每次只显示一个窗口
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    connect(m_pListW, SIGNAL(currentRowChanged(int)), m_pSW, SLOT(setCurrentIndex(int))); // 根据行号设置窗口
}

OpeWidget &OpeWidget::getInstance() {
    static OpeWidget instance;
    return instance;
}
