#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushImport_pr_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Imporing a product table"), "C://", "Excel file (*.xlsx)");
}

void MainWindow::on_pushImport_cl_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Imporing a clients table"), "C://", "Excel file (*.xlsx)");
}
