/********************************************************************************
** Form generated from reading UI file 'server_page.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVER_PAGE_H
#define UI_SERVER_PAGE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_server_pageClass
{
public:
    QAction *action1;
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QTextBrowser *text_log;
    QPushButton *connect;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *server_pageClass)
    {
        if (server_pageClass->objectName().isEmpty())
            server_pageClass->setObjectName(QString::fromUtf8("server_pageClass"));
        server_pageClass->resize(414, 316);
        QIcon icon;
        icon.addFile(QString::fromUtf8("C:/Users/Administrator/.designer/backup/images/server.ico"), QSize(), QIcon::Normal, QIcon::Off);
        server_pageClass->setWindowIcon(icon);
        server_pageClass->setAutoFillBackground(false);
        action1 = new QAction(server_pageClass);
        action1->setObjectName(QString::fromUtf8("action1"));
        centralWidget = new QWidget(server_pageClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 361, 241));
        text_log = new QTextBrowser(groupBox);
        text_log->setObjectName(QString::fromUtf8("text_log"));
        text_log->setGeometry(QRect(91, 22, 256, 201));
        connect = new QPushButton(groupBox);
        connect->setObjectName(QString::fromUtf8("connect"));
        connect->setGeometry(QRect(10, 28, 75, 23));
        connect->setStyleSheet(QString::fromUtf8("background-image: url(:/server_page/images/6.png);"));
        server_pageClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(server_pageClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 414, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        server_pageClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(server_pageClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        server_pageClass->addToolBar(Qt::LeftToolBarArea, mainToolBar);
        statusBar = new QStatusBar(server_pageClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        server_pageClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menu->addAction(action1);

        retranslateUi(server_pageClass);

        QMetaObject::connectSlotsByName(server_pageClass);
    } // setupUi

    void retranslateUi(QMainWindow *server_pageClass)
    {
        server_pageClass->setWindowTitle(QApplication::translate("server_pageClass", "server_page", nullptr));
        action1->setText(QApplication::translate("server_pageClass", "\347\253\257\345\217\243", nullptr));
        groupBox->setTitle(QApplication::translate("server_pageClass", "\346\227\245\345\277\227", nullptr));
        connect->setText(QApplication::translate("server_pageClass", "\345\220\257\345\212\250\346\234\215\345\212\241\345\231\250", nullptr));
        menu->setTitle(QApplication::translate("server_pageClass", "\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class server_pageClass: public Ui_server_pageClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVER_PAGE_H
