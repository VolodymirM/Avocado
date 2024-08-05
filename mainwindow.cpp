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

struct Client
{
    QString name;
    std::vector<QString> product_quantity;
    Client(QVariant new_name) {name = new_name.toString();}
};

QString pr_filename;
std::vector<Product> products;
std::vector<Client> clients;
std::vector<QString> palletes;
bool pr_imported = false, cl_imported = false, distributed = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableProducts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCustomers->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

    pr_imported = true;

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

    if (cl_imported) {
        clients.clear();
        palletes.clear();
    }

    cl_imported = true;

    unsigned count = 2;
    while (xlsx.cellAt(count, 1) != nullptr) {
        palletes.push_back(xlsx.cellAt(count, 1)->readValue().toString());
        qDebug() << palletes.back();
        ++count;
    }

    count = 2;
    while (xlsx.cellAt(1, count) != nullptr) {
        clients.push_back(Client(xlsx.cellAt(1, count)->readValue()));
        qDebug() << clients.back().name;
        for (size_t i = 2; i < palletes.size() + 2; ++i) {
            if (xlsx.cellAt(i, count) == nullptr)
                clients[count - 2].product_quantity.push_back("");
            else
                clients[count - 2].product_quantity.push_back(xlsx.cellAt(i, count)->readValue().toString());
        }
        ++count;
    }

    ui->tableCustomers->setColumnCount(clients.size());
    ui->tableCustomers->setRowCount(palletes.size());

    QString header;
    for (size_t i = 0; i < clients.size(); ++i)
        header += clients[i].name + ";";
    ui->tableCustomers->setHorizontalHeaderLabels(header.split(";"));

    header = "";
    for (size_t i = 0; i < palletes.size(); ++i)
        header += palletes[i] + ";";
    ui->tableCustomers->setVerticalHeaderLabels(header.split(";"));

    for (count = 0; count < clients.size(); ++count) {
        for (size_t i = 0; i < palletes.size(); ++i)
            ui->tableCustomers->setItem(i, count, new QTableWidgetItem(clients[count].product_quantity[i]));
    }

    ui->tableCustomers->resizeColumnsToContents();
    ui->tableCustomers->resizeRowsToContents();
    ui->tableCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableCustomers->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    return;
}
