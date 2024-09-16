#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <vector>
#include <unordered_map>

#include "xlsxdocument.h"
#include "xlsxworkbook.h"

const int RUN = 32;

std::vector<QString> container_names;
std::vector<QString> product_names;

struct Product
{
    unsigned line;
    size_t container_index;
    size_t name_index;
    bool distributed;
    Product(unsigned new_line, QVariant new_container, QVariant new_name) {
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

    Product() : line(0), container_index(0), name_index(0), distributed(false) {}
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
bool isEmpty(QXlsx::Document &xlsx);

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

    QXlsx::Cell* cellE1 = xlsx.cellAt(1, 5);  // E1 cell
    QXlsx::Cell* cellF1 = xlsx.cellAt(1, 6);  // F1 cell

    bool unsuitable = !cellE1 || cellF1;

    if (isEmpty(xlsx) || unsuitable) {
        QMessageBox::critical(this, "Unable to import the table",
            "WARNING: It seems like you are trying to use unsuitable table. Please, check if you are importing a table with your products.");
        return;
    }

    pr_filename = new_pr_filename;
    new_pr_filename = "";

    products.clear();
    container_names.clear();
    product_names.clear();
    ui->tableProducts->clearContents();
    ui->tableProducts->clear();

    pr_imported = true;
    distributed = false;
    saved = false;

    unsigned count = 2;
    while (xlsx.cellAt(count, 3) != nullptr) {
        products.push_back(Product(count - 1, xlsx.cellAt(count, 2)->readValue(), xlsx.cellAt(count, 4)->readValue()));
        ++count;
    }

    ui->tableProducts->setColumnCount(5);
    ui->tableProducts->setRowCount(products.size());
    ui->tableProducts->setHorizontalHeaderLabels(QString("CRT;EX-FILE - CONTAINER;PALLET #;Produce;Customer").split(";"));

    for (size_t i = 0; i < products.size(); ++i) {
        if (xlsx.cellAt(i + 2, 1) && xlsx.cellAt(i + 2, 1)->readValue().isValid())
            ui->tableProducts->setItem(i, 0, new QTableWidgetItem(xlsx.cellAt(i + 2, 1)->readValue().toString()));
        ui->tableProducts->setItem(i, 1, new QTableWidgetItem(container_names[products[i].container_index]));
        ui->tableProducts->setItem(i, 2, new QTableWidgetItem(xlsx.cellAt(i + 2, 3)->readValue().toString()));
        ui->tableProducts->setItem(i, 3, new QTableWidgetItem(product_names[products[i].name_index]));
        if (xlsx.cellAt(i + 2, 5) && xlsx.cellAt(i + 2, 5)->readValue().isValid())
            ui->tableProducts->setItem(i, 4, new QTableWidgetItem(xlsx.cellAt(i + 2, 5)->readValue().toString()));
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

    if (isEmpty(xlsx)) {
        QMessageBox::critical(this, "Unable to import the table",
            "WARNING: It seems like you are trying to use an empty table. "
            "Please, check if the table is not empty.");
        return;
    }

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
                    ui->tableProducts->setItem(it->line - 1, 4, new QTableWidgetItem(client.name));
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
    if (saved || !pr_imported)
        return;

    QXlsx::Document xlsx(pr_filename);

    for (int i = 0; i < ui->tableProducts->rowCount(); ++i) {
        QTableWidgetItem* item = ui->tableProducts->item(i, 4);
        if (item)
            xlsx.write(i + 2, 5, item->text());
        else
            xlsx.write(i + 2, 5, "");
    }

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

void insertionSort(std::vector<Product_toSort>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        Product_toSort temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j].count > temp.count) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

void merge(std::vector<Product_toSort>& arr, int left, int mid, int right) {
    int len1 = mid - left + 1, len2 = right - mid;
    std::vector<Product_toSort> leftArr(len1);
    std::vector<Product_toSort> rightArr(len2);

    for (int i = 0; i < len1; i++)
        leftArr[i] = arr[left + i];
    for (int i = 0; i < len2; i++)
        rightArr[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < len1 && j < len2) {
        if (leftArr[i].count <= rightArr[j].count) {
            arr[k] = leftArr[i];
            i++;
        } else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < len1) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < len2) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void timSort(std::vector<Product_toSort>& arr) {
    int n = arr.size();

    for (int i = 0; i < n; i += RUN)
        insertionSort(arr, i, std::min((i + RUN - 1), (n - 1)));

    for (int size = RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min((left + 2 * size - 1), (n - 1));

            if (mid < right)
                merge(arr, left, mid, right);
        }
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
    timSort(new_order);

    size_t i = 0;

    for (auto& container : new_order) {
        for (auto& element : containers_groups[container.container_index]) {
            products[i] = element;
            ++i;
        }
    }
}

bool isEmpty(QXlsx::Document &xlsx) {
    int rowCount = xlsx.dimension().rowCount();
    int colCount = xlsx.dimension().columnCount();

    for (int row = 1; row <= rowCount; ++row)
        for (int col = 1; col <= colCount; ++col) {
            QXlsx::Cell* cell = xlsx.cellAt(row, col);
            if (cell && cell->readValue().isValid() && !cell->readValue().toString().isEmpty())
                return false;
        }

    return true;
}
