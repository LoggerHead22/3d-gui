#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mls.h"
#include "DDD.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->l1LineEdit->setText(QString::number(l1));
    ui->l2LineEdit->setText(QString::number(l2));
    ui->alphaLineEdit->setText(QString::number(alpha));
    ui->kLineEdit->setText(QString::number(k));
    ui->nxLineEdit->setText(QString::number(nx));
    ui->nyLineEdit->setText(QString::number(ny));
    ui->pLineEdit->setText(QString::number(p));
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

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        lastMousePosition = event->pos();
        lastHorizontalAngle = horizontalAngle;
        lastVerticalAngle = verticalAngle;
        isRotating = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isRotating = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    const float sensitivity = 0.3;
    if (isRotating) {
        setHorizontalAngle(lastHorizontalAngle + (event->pos().x() - lastMousePosition.x()) * sensitivity);
        setVerticalAngle(lastVerticalAngle + (event->pos().y() - lastMousePosition.y()) * sensitivity);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    setZoom(zoom + event->delta() / 4000.0);
//    qDebug() << event->delta();
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

void MainWindow::setZoom(float zoom_) {
    zoom = qMax(0.3f, zoom_);
    zoom = qMin(2.0f, zoom);
    redraw();
}

void MainWindow::setParallelogram() {
    l1 = ui->l1LineEdit->text().toDouble();
    l2 = ui->l2LineEdit->text().toDouble();
    alpha = ui->alphaLineEdit->text().toDouble();
    k = ui->kLineEdit->text().toDouble();
    nx = ui->nxLineEdit->text().toInt();
    ny = ui->nyLineEdit->text().toInt();
    p = ui->pLineEdit->text().toInt();
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
