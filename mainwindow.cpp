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
    connect(ui->zoomSlider, &QSlider::valueChanged, this, &MainWindow::setZoom);

    ui->l1LineEdit->setText(QString::number(l1));
    ui->l2LineEdit->setText(QString::number(l2));
    ui->alphaLineEdit->setText(QString::number(alpha));
    ui->kLineEdit->setText(QString::number(k));
    ui->nxLineEdit->setText(QString::number(nx));
    ui->nyLineEdit->setText(QString::number(ny));
    connect(ui->computePushButton, &QPushButton::released, this, &MainWindow::compute);
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
    return rotator * QVector3D(0, 0.7, -2) / zoom;
}

void MainWindow::setHorizontalAngle(int angle) {
    horizontalAngle = angle;
    redraw();
}

void MainWindow::setVerticalAngle(int angle) {
    verticalAngle = angle;
    redraw();
}

void MainWindow::setZoom(int zoom_) {
    zoom = zoom_ / 100.0;
    redraw();
}

void MainWindow::setParallelogram() {
    l1 = ui->l1LineEdit->text().toDouble();
    l2 = ui->l2LineEdit->text().toDouble();
    alpha = ui->alphaLineEdit->text().toDouble();
    k = ui->kLineEdit->text().toDouble();
    nx = ui->nxLineEdit->text().toInt();
    ny = ui->nyLineEdit->text().toInt();
    compute();
}

void MainWindow::activate() {
    ui->computePushButton->setEnabled(true);
}

void MainWindow::deactivate() {
    ui->computePushButton->setEnabled(false);
}

void MainWindow::redraw() {
    update();
}

int MainWindow::compute() {
}
