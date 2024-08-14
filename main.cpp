#include "mainwindow.h"

#include <QApplication>

struct B{
    int first;
    QString second;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Avocado");
    w.show();
    return a.exec();
}
