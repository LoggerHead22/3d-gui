#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mls.h"
#include "DDD.h"

double f_1(double , double){
    return 1;
}
double f_2(double x, double){
    return x;
}
double f_3(double , double y){
    return y;
}
double f_4(double x, double y){
    return x + y;
}

double f_5(double x, double y){
    return x*y + x*x ;
}

double f_6(double x, double y){
    return exp(x)*exp(y);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    functions.push_back({f_1, "f(x, y) = 1"});
    functions.push_back({f_2, "f(x, y) = x"});
    functions.push_back({f_3, "f(x, y) = y"});
    functions.push_back({f_4, "f(x, y) = x + y"});
    functions.push_back({f_5, "f(x, y) = x*y + x*x"});
    functions.push_back({f_6, "f(x, y) = e^x * e^y"});

    ui->functionLabel->setText(functions[0].second);
    connect(ui->changeFunctionPushButton, &QPushButton::released, this, [&]() {
        setCurrentFunction((currentFunctionIndex + 1) % functions.size());
    });

    ui->l1LineEdit->setText(QString::number(l1));
    ui->l2LineEdit->setText(QString::number(l2));
    ui->alphaLineEdit->setText(QString::number(alpha));
    ui->kLineEdit->setText(QString::number(k));
    ui->nxLineEdit->setText(QString::number(nx));
    ui->nyLineEdit->setText(QString::number(ny));
    ui->pLineEdit->setText(QString::number(p));
    connect(ui->computePushButton, &QPushButton::released, this, &MainWindow::compute);

    computeTimer.setInterval(1000);
//    computeTimer.callOnTimeout(this, &MainWindow::compute);
    computeTimer.start();
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
}

QVector3D MainWindow::getEye() {
    QMatrix4x4 rotator;
    rotator.rotate(horizontalAngle, {0, 1, 0});
    rotator.rotate(verticalAngle, {1, 0, 0});
    return rotator * QVector3D(0, 0.7, -2) / zoom;
}

MainWindow::XXX MainWindow::currentFunction() {
    return functions[currentFunctionIndex].first;
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

void MainWindow::setCurrentFunction(int index) {
    currentFunctionIndex = index;
    ui->functionLabel->setText(functions[currentFunctionIndex].second);
    compute();
}

void MainWindow::activate() {
    ui->computePushButton->setEnabled(true);
    ui->changeFunctionPushButton->setEnabled(true);
}

void MainWindow::deactivate() {
    ui->computePushButton->setEnabled(false);
    ui->changeFunctionPushButton->setEnabled(false);
}

void MainWindow::redraw() {
    update();
}

int MainWindow::compute() {
    redraw();
    return 0;
}
