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

    QVector3D eye = {0, 0.7, -2};

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
