#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <QPainter>
#include <QMouseEvent>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>
#include <QTime>
#include <QPair>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    QVector3D getEye();
    void setHorizontalAngle(int angle);
    void setVerticalAngle(int angle);
    void setZoom(int zoom);
    void setParallelogram();
    void activate();
    void deactivate();
    void redraw();
    int compute();

    // поворот мышкой
    QPoint lastMousePosition;
    bool isRotating = false;
    float lastHorizontalAngle, lastVerticalAngle;

    // камера
    float zoom = 1;
    int horizontalAngle = 0, verticalAngle = 0;

    // параллелограм
    double l1 = 3, l2 = 5, alpha = 60, k = 0.2;
    int nx = 6, ny = 12;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
