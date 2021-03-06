#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>
#include <QTimer>
#include <QPair>
#include <QKeyEvent>
#include "mls.h"
#include "triangle.h"
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int bs_x , int bs_y , int p, double l1, double l2, double alpha, double k, QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    QVector3D getEye();
    using XXX = double(*)(double, double);
    XXX currentFunction();
    void setHorizontalAngle(int angle);
    void setVerticalAngle(int angle);
    void setZoom(float zoom_);
    void setParallelogram();
    void setCurrentFunction(int index);
    void activate();
    void deactivate();
    void redraw();
    int compute();
    void funcRange();
    void changeKit();
    double f_aprox_value(double x , double y);
    vector<Triangle> func_apr_trio( const QVector3D& size );
    vector<Triangle> func_resid_trio(const QVector3D& size);
    // поворот мышкой
    QPoint lastMousePosition;
    bool isRotating = false;
    float lastHorizontalAngle, lastVerticalAngle;

    // камера
    float zoom = 1;
    int horizontalAngle = 0, verticalAngle = 0;

    // graph
    QPair<double, double> xRange, yRange, zRange;

    // апроксимация

    int nx = 6, ny = 6;
    const int base_nx , base_ny;
    int p;
    double l1 = 3, l2 = 5, alpha = 90, k = 0.2;
    parral par;
    const int base_nx_rect , base_ny_rect;
    double residual , time;
    int kit = 0;
    bool isActive = false;


    QVector<QPair<double(*)(double, double), QString>> functions;
    int currentFunctionIndex = 0;
    QTimer computeTimer;

    // потоки
    int *error = nullptr;

    double *a=nullptr, *b=nullptr, *x=nullptr , *v=nullptr , *u=nullptr , *r=nullptr , *buf=nullptr ,*x_copy = nullptr;
    int * I=nullptr;
    pthread_t *tids=nullptr;
    Arg *args=nullptr;
    int N;
    vector<Triangle> func_trio ,apr_trio , resid_trio;
    double func_max, func_min;

    void allocThreadVars();
    void freeThreadVars();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
