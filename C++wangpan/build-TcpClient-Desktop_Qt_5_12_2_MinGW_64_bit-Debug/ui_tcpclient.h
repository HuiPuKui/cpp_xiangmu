/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *name_lab;
    QLineEdit *name_le;
    QHBoxLayout *horizontalLayout_3;
    QLabel *pwd_lab;
    QLineEdit *pwd_le;
    QPushButton *login_pb;
    QHBoxLayout *horizontalLayout;
    QPushButton *regist_pb;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancel_pb;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(486, 290);
        verticalLayout_2 = new QVBoxLayout(TcpClient);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        name_lab = new QLabel(TcpClient);
        name_lab->setObjectName(QString::fromUtf8("name_lab"));
        QFont font;
        font.setFamily(QString::fromUtf8("Agency FB"));
        font.setPointSize(20);
        name_lab->setFont(font);

        horizontalLayout_2->addWidget(name_lab);

        name_le = new QLineEdit(TcpClient);
        name_le->setObjectName(QString::fromUtf8("name_le"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(20);
        name_le->setFont(font1);

        horizontalLayout_2->addWidget(name_le);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pwd_lab = new QLabel(TcpClient);
        pwd_lab->setObjectName(QString::fromUtf8("pwd_lab"));
        pwd_lab->setFont(font);

        horizontalLayout_3->addWidget(pwd_lab);

        pwd_le = new QLineEdit(TcpClient);
        pwd_le->setObjectName(QString::fromUtf8("pwd_le"));
        pwd_le->setFont(font1);
        pwd_le->setEchoMode(QLineEdit::Password);

        horizontalLayout_3->addWidget(pwd_le);


        verticalLayout->addLayout(horizontalLayout_3);

        login_pb = new QPushButton(TcpClient);
        login_pb->setObjectName(QString::fromUtf8("login_pb"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Agency FB"));
        font2.setPointSize(24);
        login_pb->setFont(font2);

        verticalLayout->addWidget(login_pb);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        regist_pb = new QPushButton(TcpClient);
        regist_pb->setObjectName(QString::fromUtf8("regist_pb"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Agency FB"));
        font3.setPointSize(16);
        regist_pb->setFont(font3);

        horizontalLayout->addWidget(regist_pb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cancel_pb = new QPushButton(TcpClient);
        cancel_pb->setObjectName(QString::fromUtf8("cancel_pb"));
        cancel_pb->setFont(font3);

        horizontalLayout->addWidget(cancel_pb);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QApplication::translate("TcpClient", "TcpClient", nullptr));
        name_lab->setText(QApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        pwd_lab->setText(QApplication::translate("TcpClient", "\345\257\206     \347\240\201\357\274\232", nullptr));
        login_pb->setText(QApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        regist_pb->setText(QApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        cancel_pb->setText(QApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
