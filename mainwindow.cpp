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

std::vector<QString> container_names;
std::vector<QString> product_names;

struct Product
{
    unsigned line;
    size_t container_index;
    QString pallet;
    size_t name_index;
    QString customer;
    Product(unsigned new_line, QVariant new_container, QVariant new_pallet, QVariant new_name) {
        line = new_line;

        bool is_new = true;

        for (size_t i = 0; i < container_names.size(); ++i) {
            if (container_names[i] == new_container.toString()) {
                is_new = false;
                container_index = i;
                break;
            }
        }
        if (is_new) {
            container_names.push_back(new_container.toString());
            container_index = container_names.size() - 1;
        }

        pallet = new_pallet.toString();

        is_new = true;
        for (size_t i = 0; i < product_names.size(); ++i) {
            if (product_names[i] == new_name.toString()) {
                is_new = false;
                name_index = i;
                break;
            }
        }
        if (is_new) {
            product_names.push_back(new_name.toString());
            name_index = product_names.size() - 1;
        }

        customer = "";
    }

    Product() : line(0), container_index(0), name_index(0), customer("") {}
};

struct to_export {
    size_t index;
    unsigned amount;
    to_export(size_t new_index, unsigned new_ammount) : index(new_index), amount(new_ammount) {}
};

struct Client
{
    QString name;
    std::vector<to_export> product_quantity;
    Client(QVariant new_name) {name = new_name.toString();}
    Client() : name("") {}
};

bool pr_imported = false, cl_imported = false, distributed = false;

QString pr_filename;
std::vector<Product> products;

std::vector<Client> clients;
std::vector<QString> palletes;

void merge_productsByContainer(std::vector<std::string>& products, int left, int mid, int right);
void mergeSort_productsByContainer(std::vector<Product>& products, int left, int right);
void merge_clientsByProducts(std::vector<Client>& clients, int left, int mid, int right);
void mergeSort_clientsByProducts(std::vector<Client>& clients, int left, int right);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->importProductData, &QPushButton::clicked, this, &MainWindow::importProductData);
    connect(ui->importCustomerData, &QPushButton::clicked, this, &MainWindow::importCustomerData);
    connect(ui->distributeProducts, &QPushButton::clicked, this, &MainWindow::distributeProducts);

    ui->tableProducts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCustomers->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::importProductData()
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

    if (pr_imported) {
        products.clear();
        container_names.clear();
        product_names.clear();
    }

    pr_imported = true;
    distributed = false;

    unsigned count = 2;
    while (xlsx.cellAt(count, 4) != nullptr) {
        products.push_back(Product(count - 1, xlsx.cellAt(count, 3)->readValue(), xlsx.cellAt(count, 4)->readValue(), xlsx.cellAt(count, 5)->readValue()));
        qDebug() << products.back().line << " " << container_names[products.back().container_index] << " " <<
            products.back().pallet << " " << product_names[products.back().name_index];
        ++count;
    }

    ui->tableProducts->setColumnCount(4);
    ui->tableProducts->setRowCount(products.size());
    ui->tableProducts->setHorizontalHeaderLabels(QString("EX-FILE - CONTAINER;PALLET #;Produce;Customer").split(";"));

    for (size_t i = 0; i < products.size(); ++i) {
        ui->tableProducts->setItem(i, 0, new QTableWidgetItem(container_names[products[i].container_index]));
        ui->tableProducts->setItem(i, 1, new QTableWidgetItem(products[i].pallet));
        ui->tableProducts->setItem(i, 2, new QTableWidgetItem(product_names[products[i].name_index]));
    }

    ui->tableProducts->resizeColumnsToContents();
    ui->tableProducts->resizeRowsToContents();
    ui->tableProducts->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableProducts->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    return;
}

void MainWindow::importCustomerData()
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

    QXlsx::Cell* test_empty1 = xlsx.cellAt(2, 1); // Cell B1
    QXlsx::Cell* test_empty2 = xlsx.cellAt(1, 2); // Cell A2
    bool isEmpty1Valid = !(test_empty1 && test_empty1->readValue().isValid());
    bool isEmpty2Valid = !(test_empty2 && test_empty2->readValue().isValid());

    if (isEmpty1Valid || isEmpty2Valid) {
        QMessageBox::critical(this, "Unable to import the table",
            "WARNING: It seems like you are trying to use unsuitable table, or your table is empty. "
            "Please, check if you are importing a table with your customers, and the table is not empty.");
        qDebug() << "Unsuitable table";
        test_empty1 = nullptr;
        test_empty2 = nullptr;
        return;
    }
    qDebug() << "Successfully opened and checked the Excel file.";

    test_empty1 = nullptr;
    test_empty2 = nullptr;

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
            if (xlsx.cellAt(i, count)->readValue().isValid())
                clients[count - 2].product_quantity.push_back(to_export(i - 2, xlsx.cellAt(i, count)->readValue().toInt()));

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
        for (to_export element : clients[count].product_quantity) {
            ui->tableCustomers->setItem(element.index, count, new QTableWidgetItem(QString::number(element.amount)));
            qDebug() << element.index << " Ammount: "  << element.amount;
        }
    }

    ui->tableCustomers->resizeColumnsToContents();
    ui->tableCustomers->resizeRowsToContents();
    ui->tableCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableCustomers->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    return;
}

void MainWindow::distributeProducts()
{
    if(!pr_imported || !cl_imported || distributed) {
        qDebug() << "Distribution failed";
        return;
    }

    mergeSort_productsByContainer(products, 0, products.size() - 1);
    mergeSort_clientsByProducts(clients, 0, clients.size() - 1);


    for (Client& client : clients) {
        for (to_export& element : client.product_quantity) {
            auto it = products.begin();
            while (it != products.end()) {
                if (palletes[element.index] == product_names[it->name_index] && it->customer == "") {
                    ui->tableProducts->setItem(it->line - 1, 3, new QTableWidgetItem(client.name));
                    it->customer = client.name;
                    --(element.amount);
                    qDebug() << it->line << " " << container_names[it->container_index] << " " <<
                        it->pallet << " " << product_names[it->name_index] << " " << it->customer;
                    if (element.amount == 0)
                        break;
                }
                ++it;
            }
        }
    }

    ui->tableCustomers->resizeColumnsToContents();
    ui->tableCustomers->resizeRowsToContents();
    ui->tableCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableCustomers->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    distributed = true;
    qDebug() << "Distributed";
}

void merge_productsByContainer(std::vector<Product>& products, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<Product> L(n1);
    std::vector<Product> R(n2);

    for (int i = 0; i < n1; ++i)
        L[i] = products[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = products[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;
    while (i < n1 && j < n2) {
        if (L[i].container_index <= R[j].container_index) {
            products[k] = L[i];
            ++i;
        } else {
            products[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        products[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        products[k] = R[j];
        ++j;
        ++k;
    }
}

void mergeSort_productsByContainer(std::vector<Product>& products, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort_productsByContainer(products, left, mid);
        mergeSort_productsByContainer(products, mid + 1, right);

        merge_productsByContainer(products, left, mid, right);
    }
}

void merge_clientsByProducts(std::vector<Client>& clients, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<Client> L(n1);
    std::vector<Client> R(n2);

    for (int i = 0; i < n1; ++i)
        L[i] = clients[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = clients[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;
    while (i < n1 && j < n2) {
        unsigned left_ammount = 0, right_ammount = 0;

        for (auto product : L[i].product_quantity)
            left_ammount += product.amount;
        for (auto product : R[j].product_quantity)
            right_ammount += product.amount;

        if (left_ammount <= right_ammount) {
            clients[k] = L[i];
            ++i;
        } else {
            clients[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        clients[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        clients[k] = R[j];
        ++j;
        ++k;
    }
}

void mergeSort_clientsByProducts(std::vector<Client>& clients, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort_clientsByProducts(clients, left, mid);
        mergeSort_clientsByProducts(clients, mid + 1, right);

        merge_clientsByProducts(clients, left, mid, right);
    }
}
