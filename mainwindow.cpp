#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <vector>
#include <unordered_map>

#include "xlsxdocument.h"
#include "xlsxworkbook.h"

std::vector<QString> container_names;
std::vector<QString> product_names;

struct Product
{
    unsigned line;
    size_t container_index;
    QString pallet;
    size_t name_index;
    bool distributed;
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

        distributed = false;
    }

    Product() : line(0), container_index(0), pallet(""), name_index(0), distributed(false) {}
};

struct Product_toSort {
    size_t container_index;
    unsigned count;
    Product_toSort() : container_index(0), count(0) {}
    Product_toSort(size_t new_index, unsigned new_count) : container_index(new_index), count(new_count) {}
};

struct to_export {
    size_t index;
    unsigned ammount;
    to_export(size_t new_index, unsigned new_ammount) : index(new_index), ammount(new_ammount) {}
};

struct Client
{
    QString name;
    std::vector<to_export> product_quantity;
    Client(QVariant new_name) {name = new_name.toString();}
    Client() : name("") {}
};

bool pr_imported = false, cl_imported = false, distributed = false, saved = false;

QString pr_filename = "";
std::vector<Product> products;

std::vector<Client> clients;
std::vector<QString> palletes;

void newOrder_Products(std::vector<Product>& products);
void merge_clientsByProducts(std::vector<Client>& clients, int left, int mid, int right);
void mergeSort_clientsByProducts(std::vector<Client>& clients, int left, int right);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::actionSave);
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
    QString new_pr_filename = QFileDialog::getOpenFileName(this,
        tr("Importing a product table"), "C://", "Excel file (*.xlsx)");

    if (new_pr_filename.isEmpty())
        return;

    QXlsx::Document xlsx(new_pr_filename);
    if (!xlsx.load()) {
        QMessageBox::critical(this, "Error", "Failed to load the Excel file.");
        return;
    }

    QXlsx::Cell* test_suitable = xlsx.cellAt(1, 7); // Cell G2
    QXlsx::Cell* test_used = xlsx.cellAt(2, 6); // Cell F2
    QXlsx::Cell* test_empty = xlsx.cellAt(1, 2); // Cell B1

    bool isSuitableValid = test_suitable && test_suitable->readValue().isValid();
    bool isUsedValid = test_used && test_used->readValue().isValid();
    bool isEmptyValid = !(test_empty && test_empty->readValue().isValid());

    if (isUsedValid || isSuitableValid || isEmptyValid) {
        QMessageBox::critical(this, "Unable to import the table",
            "WARNING: It seems like you are trying to use unsuitable table, or your table was already used. "
            "Please, check if you are importing a table with your products, the table is not empty, it has no records below the \"F2\" cell.");
        test_suitable = nullptr;
        test_used = nullptr;
        test_empty = nullptr;
        return;
    }

    pr_filename = new_pr_filename;
    new_pr_filename = "";

    test_suitable = nullptr;
    test_used = nullptr;
    test_empty = nullptr;

    products.clear();
    container_names.clear();
    product_names.clear();
    ui->tableProducts->clearContents();
    ui->tableProducts->clear();

    pr_imported = true;
    distributed = false;
    saved = false;

    unsigned count = 2;
    while (xlsx.cellAt(count, 4) != nullptr) {
        products.push_back(Product(count - 1, xlsx.cellAt(count, 3)->readValue(), xlsx.cellAt(count, 4)->readValue(), xlsx.cellAt(count, 5)->readValue()));
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

    if (cs_filename.isEmpty())
        return;

    QXlsx::Document xlsx(cs_filename);

    if (!xlsx.load()) {
        QMessageBox::critical(this, "Error", "Failed to load the Excel file.");
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
        test_empty1 = nullptr;
        test_empty2 = nullptr;
        return;
    }

    test_empty1 = nullptr;
    test_empty2 = nullptr;

    clients.clear();
    palletes.clear();
    ui->tableCustomers->clearContents();
    ui->tableCustomers->clear();

    cl_imported = true;
    saved = false;

    if (distributed) {
        for (auto& element : products)
            element.distributed = false;

        for (size_t i = 0; i < products.size(); ++i)
            ui->tableProducts->setItem(i, 3, new QTableWidgetItem(""));

        distributed = false;
    }

    unsigned count = 2;
    while (xlsx.cellAt(count, 1) != nullptr) {
        palletes.push_back(xlsx.cellAt(count, 1)->readValue().toString());
        ++count;
    }

    count = 2;
    while (xlsx.cellAt(1, count) != nullptr) {
        clients.push_back(Client(xlsx.cellAt(1, count)->readValue()));
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
        for (to_export element : clients[count].product_quantity)
            ui->tableCustomers->setItem(element.index, count, new QTableWidgetItem(QString::number(element.ammount)));
    }

    ui->tableCustomers->resizeColumnsToContents();
    ui->tableCustomers->resizeRowsToContents();
    ui->tableCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableCustomers->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    return;
}

void MainWindow::distributeProducts()
{
    if(!pr_imported || !cl_imported || distributed)
        return;

    std::vector<Product> products_copy = products;
    std::vector<Client> clients_copy = clients;

    newOrder_Products(products_copy);
    mergeSort_clientsByProducts(clients_copy, 0, clients_copy.size() - 1);

    bool not_enough_products = false;

    for (Client& client : clients_copy) {
        for (to_export& element : client.product_quantity) {
            auto it = products_copy.begin();
            while (it <= products_copy.end()) {
                if (it == products_copy.end()) {
                    not_enough_products = true;
                    break;
                }
                if (palletes[element.index] == product_names[it->name_index] && it->distributed == false) {
                    ui->tableProducts->setItem(it->line - 1, 3, new QTableWidgetItem(client.name));
                    it->distributed = true;
                    --(element.ammount);
                    if (element.ammount == 0)
                        break;
                }
                ++it;
            }
        }
    }

    if (not_enough_products)
        QMessageBox::warning(this, "Not enough products", "There are some customers that may get not all the products they request.");

    ui->tableCustomers->resizeColumnsToContents();
    ui->tableCustomers->resizeRowsToContents();
    ui->tableCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableCustomers->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    distributed = true;

    return;
}

void MainWindow::actionSave() {
    if (saved)
        return;

    QXlsx::Document xlsx(pr_filename);
    for (auto& element : products)
        xlsx.write(element.line + 1, 6, ui->tableProducts->item(element.line - 1, 3)->text());

    xlsx.save();
    saved = true;
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
        unsigned left_ammount = 0;
        for (auto product : L[i].product_quantity)
            left_ammount += product.ammount;

        unsigned right_ammount = 0;
        for (auto product : R[j].product_quantity)
            right_ammount += product.ammount;

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

void swap(std::vector<Product_toSort>& arr, size_t i, size_t j) {
    Product_toSort temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

int partition(std::vector<Product_toSort>& arr, size_t low, size_t high) {
    size_t pivot = arr[high].count;
    size_t i = low - 1;

    for (size_t j = low; j < high; ++j) {
        if (arr[j].count <= pivot) {
            ++i;
            swap(arr, i, j);
        }
    }

    swap(arr, i + 1, high);
    return i + 1;
}

void quickSort(std::vector<Product_toSort>& arr, size_t low, size_t high) {
    if (low < high) {
        int pivotIndex = partition(arr, low, high);
        quickSort(arr, low, pivotIndex - 1);
        quickSort(arr, pivotIndex + 1, high);
    }
}

void newOrder_Products(std::vector<Product>& products) {
    std::unordered_map<size_t, std::vector<Product>> containers_groups;
    std::unordered_map<size_t, unsigned> count;

    for (auto& element : products) {
        containers_groups[element.container_index].push_back(element);
        ++count[element.container_index];
    }

    std::vector<Product_toSort> new_order;

    for (auto it = count.begin(); it != count.end(); ++it)
        new_order.push_back(Product_toSort(it->first, it->second));

    count.clear();
    quickSort(new_order, 0, new_order.size() - 1);

    size_t i = 0;

    for (auto& container : new_order) {
        for (auto& element : containers_groups[container.container_index]) {
            products[i] = element;
            ++i;
        }
    }
}
