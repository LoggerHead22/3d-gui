#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <QPainter>
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

    QVector3D getEye();
    void setHorizontalAngle(int angle);
    void setVerticalAngle(int angle);
    void redraw();

    int compute();

    int horizontalAngle = 0, verticalAngle = 0;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
