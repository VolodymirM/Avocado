#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTableWidget>
#include <vector>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

struct Product
{
    QString container;
    QString pallet;
    QString name;
    QString customer;
    Product(QVariant new_container, QVariant new_pallet, QVariant new_name) {
        container = new_container.toString();
        pallet = new_pallet.toString();
        name = new_name.toString();
    }
};

struct Customer
{
    QString name;
    std::vector<int> product_quantity;
    Customer(QVariant new_name) {name = new_name.toString();}
};

QString pr_filename;
std::vector<Product> products;
std::vector<Customer> customers;
bool pr_imported = false, cl_imported = false, distributed = false;


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

    if (pr_imported)
        products.clear();

    unsigned count = 2;
    while (xlsx.cellAt(count, 4) != nullptr) {
        products.push_back(Product(xlsx.cellAt(count, 3)->readValue(), xlsx.cellAt(count, 4)->readValue(), xlsx.cellAt(count, 5)->readValue()));
        qDebug() << products[count - 2].container << " " << products[count - 2].pallet << " " << products[count - 2].name;
        ++count;
    }

    ui->tableProducts->setColumnCount(4);
    ui->tableProducts->setRowCount(products.size());
    ui->tableProducts->setHorizontalHeaderLabels(QString("EX-FILE - CONTAINER;PALLET #;Produce;Customer").split(";"));

    for (count = 0; count < products.size(); ++count) {
        ui->tableProducts->setItem(count, 0, new QTableWidgetItem(products[count].container));
        ui->tableProducts->setItem(count, 1, new QTableWidgetItem(products[count].pallet));
        ui->tableProducts->setItem(count, 2, new QTableWidgetItem(products[count].name));
    }

    ui->tableProducts->resizeColumnsToContents();
    ui->tableProducts->resizeRowsToContents();
    ui->tableProducts->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableProducts->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    pr_imported = true;
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
