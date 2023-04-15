#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);

    static ShareFile &getInstance();

    void test();

    void updateFriend(QListWidget *pFriendList);

signals:

public slots:
    void cancelSelect();
    void selectAll();

private:
    QPushButton *m_pSelectAllPB;    // 全选
    QPushButton *m_pCancelSelectPB; // 取消全选

    QPushButton *m_pOKPB;           // 确定
    QPushButton *m_pCancelPB;       // 取消

    QScrollArea *m_pSA;             // 展示区域
    QWidget *m_pFriendW;
    QVBoxLayout *m_pFriendWVBL;
    QButtonGroup *m_pButtonGroup;

};

#endif // SHAREFILE_H
