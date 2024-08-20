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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSave;
    QAction *actionSave_as;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *labelProduct;
    QTableWidget *tableProducts;
    QPushButton *importProductData;
    QSpacerItem *horizontalSpacer_3;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_4;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QTableWidget *tableCustomers;
    QPushButton *importCustomerData;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *distributeProducts;
    QSpacerItem *horizontalSpacer_8;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_2;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(721, 501);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName("actionSave");
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave));
        actionSave->setIcon(icon);
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName("actionSave_as");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName("layoutWidget");
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        labelProduct = new QLabel(layoutWidget);
        labelProduct->setObjectName("labelProduct");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelProduct->sizePolicy().hasHeightForWidth());
        labelProduct->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(labelProduct);

        tableProducts = new QTableWidget(layoutWidget);
        tableProducts->setObjectName("tableProducts");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Ignored);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tableProducts->sizePolicy().hasHeightForWidth());
        tableProducts->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(tableProducts);

        importProductData = new QPushButton(layoutWidget);
        importProductData->setObjectName("importProductData");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(importProductData->sizePolicy().hasHeightForWidth());
        importProductData->setSizePolicy(sizePolicy2);

        verticalLayout->addWidget(importProductData);


        horizontalLayout->addLayout(verticalLayout);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        splitter->addWidget(layoutWidget);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName("layoutWidget1");
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName("label_2");
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        verticalLayout_2->addWidget(label_2);

        tableCustomers = new QTableWidget(layoutWidget1);
        tableCustomers->setObjectName("tableCustomers");
        sizePolicy1.setHeightForWidth(tableCustomers->sizePolicy().hasHeightForWidth());
        tableCustomers->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(tableCustomers);

        importCustomerData = new QPushButton(layoutWidget1);
        importCustomerData->setObjectName("importCustomerData");
        sizePolicy2.setHeightForWidth(importCustomerData->sizePolicy().hasHeightForWidth());
        importCustomerData->setSizePolicy(sizePolicy2);

        verticalLayout_2->addWidget(importCustomerData);


        horizontalLayout_2->addLayout(verticalLayout_2);

        splitter->addWidget(layoutWidget1);

        verticalLayout_3->addWidget(splitter);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer = new QSpacerItem(37, 17, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_5);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_7);

        distributeProducts = new QPushButton(centralwidget);
        distributeProducts->setObjectName("distributeProducts");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(distributeProducts->sizePolicy().hasHeightForWidth());
        distributeProducts->setSizePolicy(sizePolicy3);

        horizontalLayout_3->addWidget(distributeProducts);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_8);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_6);

        horizontalSpacer_2 = new QSpacerItem(37, 17, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 721, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(distributeProducts, importProductData);
        QWidget::setTabOrder(importProductData, importCustomerData);
        QWidget::setTabOrder(importCustomerData, tableProducts);
        QWidget::setTabOrder(tableProducts, tableCustomers);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionSave);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        actionSave_as->setText(QCoreApplication::translate("MainWindow", "Save as", nullptr));
        labelProduct->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:11pt; font-weight:700;\">Products table:</span></p></body></html>", nullptr));
        importProductData->setText(QCoreApplication::translate("MainWindow", "Import a products table (.xlsx)", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:11pt; font-weight:700;\">Customers table:</span></p></body></html>", nullptr));
        importCustomerData->setText(QCoreApplication::translate("MainWindow", "Import a customers table (.xlsx)", nullptr));
        distributeProducts->setText(QCoreApplication::translate("MainWindow", "Distribute", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
