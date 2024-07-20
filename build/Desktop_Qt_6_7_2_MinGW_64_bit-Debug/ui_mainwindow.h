/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSave;
    QAction *actionSave_as;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushImport_pr;
    QTableView *tableProduct;
    QSpacerItem *horizontalSpacer_2;
    QTableView *tableClients;
    QPushButton *pushImport_cl;
    QPushButton *pushDistribute;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1003, 629);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName("actionSave");
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName("actionSave_as");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 0, 1, 1);

        pushImport_pr = new QPushButton(centralwidget);
        pushImport_pr->setObjectName("pushImport_pr");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushImport_pr->sizePolicy().hasHeightForWidth());
        pushImport_pr->setSizePolicy(sizePolicy);

        gridLayout->addWidget(pushImport_pr, 1, 0, 1, 1);

        tableProduct = new QTableView(centralwidget);
        tableProduct->setObjectName("tableProduct");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tableProduct->sizePolicy().hasHeightForWidth());
        tableProduct->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(tableProduct, 0, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 2, 1, 1);

        tableClients = new QTableView(centralwidget);
        tableClients->setObjectName("tableClients");
        sizePolicy1.setHeightForWidth(tableClients->sizePolicy().hasHeightForWidth());
        tableClients->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(tableClients, 0, 2, 1, 1);

        pushImport_cl = new QPushButton(centralwidget);
        pushImport_cl->setObjectName("pushImport_cl");
        sizePolicy.setHeightForWidth(pushImport_cl->sizePolicy().hasHeightForWidth());
        pushImport_cl->setSizePolicy(sizePolicy);

        gridLayout->addWidget(pushImport_cl, 1, 2, 1, 1);

        pushDistribute = new QPushButton(centralwidget);
        pushDistribute->setObjectName("pushDistribute");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushDistribute->sizePolicy().hasHeightForWidth());
        pushDistribute->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(pushDistribute, 2, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1003, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_as);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        actionSave_as->setText(QCoreApplication::translate("MainWindow", "Save as", nullptr));
        pushImport_pr->setText(QCoreApplication::translate("MainWindow", "Import a product table (.xlsx)", nullptr));
        pushImport_cl->setText(QCoreApplication::translate("MainWindow", "Import a clients table (.xlsx)", nullptr));
        pushDistribute->setText(QCoreApplication::translate("MainWindow", "Distribute", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File (Product table)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
