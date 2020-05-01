#include "mainwindow.h"
#include <iostream>
#include <QApplication>
#include <QMessageBox>
#include <fstream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    if (argc != 5) {
//        QMessageBox::critical(nullptr, "Invalid argument count", "Usage: file nx ny p");
//        exit(-1);
//    }
//    int nx, ny, p;
//    try {
//        nx = std::stoi(argv[2]);
//        ny = std::stoi(argv[3]);
//        p = std::stoi(argv[4]);
//    } catch (...) {
//        QMessageBox::critical(nullptr, "Invalid arguments", "Usage: file nx ny p");
//        exit(-2);
//    }
//    if (nx <= 0 || ny <= 0 || p <= 0) {
//        QMessageBox::critical(nullptr, "Invalid arguments", "Usage: file nx ny p");
//        exit(-3);
//    }

//    std::ifstream file(argv[1]);
//    if (!file) {
//        QMessageBox::critical(nullptr, "Invalid file", "Usage: file nx ny p");
//        exit(-4);
//    }
//    double l1, l2, alpha, k;
//    file>> l1>> l2>> alpha>> k;

//    MainWindow w(nx, ny, p, l1, l2, alpha, k);

    MainWindow w(3, 4, 3, 3, 5, 60, 0.4);
    w.show();
    return a.exec();
}
