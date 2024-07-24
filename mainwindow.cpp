#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

QString pr_filename;

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
    pr_filename = QFileDialog::getOpenFileName(this,
        tr("Importing a product table"), "C://", "Excel file (*.xlsx)");
    if (pr_filename.isEmpty()) {
        qDebug() << "No file selected";
        return;
    }
    QXlsx::Document xlsx(pr_filename);
    if (!xlsx.load()) {
        QMessageBox::critical(this, "Error", "Failed to load the Excel file.");
        qDebug() << "Failed to load the Excel file.";
        return;
    }
    QXlsx::Cell* test_suitable = xlsx.cellAt(1, 7); // Cell G2
    QXlsx::Cell* test_used = xlsx.cellAt(2, 6); // Cell F2
    QXlsx::Cell* test_empty = xlsx.cellAt(1, 1); // Cell A1
    bool isSuitableValid = test_suitable && test_suitable->readValue().isValid();
    bool isUsedValid = test_used && test_used->readValue().isValid();
    bool isEmptyValid = !(test_empty && test_empty->readValue().isValid());
    if (isUsedValid || isSuitableValid || isEmptyValid) {
        QMessageBox::critical(this, "Unable to import the table",
            "WARNING: It seems like you are trying to use unsuitable table, or your table was already used. "
            "Please, check if you are importing a table with your products, the table is not empty, it has no records below the \"F2\" cell.");
        qDebug() << "Unsuitable table";
        test_suitable = nullptr;
        test_used = nullptr;
        test_empty = nullptr;
        return;
    }
    qDebug() << "Successfully opened and checked the Excel file.";
    test_suitable = nullptr;
    test_used = nullptr;
    test_empty = nullptr;

    //Continue here
    return;
}


void MainWindow::on_pushImport_cs_clicked()
{
    QString cs_filename = QFileDialog::getOpenFileName(this,
        tr("Importing a customer table"), "C://", "Excel file (*.xlsx)");
    if (cs_filename.isEmpty()) {
        qDebug() << "No file selected";
        return;
    }
    QXlsx::Document xlsx(cs_filename);
    if (!xlsx.load()) {
        QMessageBox::critical(this, "Error", "Failed to load the Excel file.");
        qDebug() << "Failed to load the Excel file.";
        return;
    }
    QXlsx::Cell* test_suitable = xlsx.cellAt(1, 7); // Cell G1
    QXlsx::Cell* test_empty = xlsx.cellAt(1, 2); // Cell A1
    bool isSuitableValid = !(test_suitable && test_suitable->readValue().isValid());
    bool isEmptyValid = !(test_empty && test_empty->readValue().isValid());
    if (isSuitableValid || isEmptyValid) {
        QMessageBox::critical(this, "Unable to import the table",
            "WARNING: It seems like you are trying to use unsuitable table, or your table is empty. "
            "Please, check if you are importing a table with your customers, and the table is not empty.");
        qDebug() << "Unsuitable table";
        test_suitable = nullptr;
        test_empty = nullptr;
        return;
    }
    qDebug() << "Successfully opened and checked the Excel file.";
    test_suitable = nullptr;
    test_empty = nullptr;

    // Continue here
    return;
}
