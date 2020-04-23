#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mls.h"
#include "DDD.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &MainWindow::setHorizontalAngle);
    connect(ui->verticalSlider, &QSlider::valueChanged, this, &MainWindow::setVerticalAngle);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    DDD ddd(painter, *this, getEye());

    ddd.drawAxes({3, 3, 3}, {0, 10}, 2, {10, 20}, 3, {20, 30}, 4);
}

QVector3D MainWindow::getEye() {
    QMatrix4x4 rotator;
    rotator.rotate(horizontalAngle, {0, 1, 0});
    rotator.rotate(verticalAngle, {1, 0, 0});
    return rotator * QVector3D(0, 0.7, -2);
}

void MainWindow::setHorizontalAngle(int angle) {
    horizontalAngle = angle;
    redraw();
}

void MainWindow::setVerticalAngle(int angle) {
    verticalAngle = angle;
    redraw();
}

void MainWindow::redraw() {
    update();
}

int MainWindow::compute() {
    redraw();
    return 0;
}
