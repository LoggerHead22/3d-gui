#include "mainwindow.h"
#include "ui_mainwindow.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <QPainter>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>
#include <QTime>
#include <QPair>

struct DDD {
    QPainter& painter;
    const float zNear;
    const float zFar;
    const float fov;
    const float width, height;
    const float a;
    const QVector3D eye, center;
    QMatrix4x4 transform;

    DDD(QPainter& painter, const QWidget& widget, float zNear, float zFar, float fov, const QVector3D& eye, const QVector3D& center)
        : painter(painter)
        , zNear(zNear)
        , zFar(zFar)
        , fov(fov)
        , width(widget.size().width())
        , height(widget.size().height())
        , a(width / height)
        , eye(eye)
        , center(center)
    {
        // view
        transform.scale({width / 2, height / 2, 1});
        transform.translate({1, 1, 0});
        transform.scale({-1, -1, -1});

        // projection
        transform.perspective(fov, a, zNear, zFar);

        // camera
        transform.lookAt(eye, center, {0, 1, 0});
        transform.translate(-eye);
    }

    DDD(QPainter& painter, const QWidget& widget, const QVector3D& eye, const QVector3D& center)
        : DDD(painter, widget, 0.1, 1000, 70, eye, center)
    {}

    QPointF project(const QVector3D& p) {
        QVector4D q(p, 1);
        q = transform * q;
        q /= q.w();
        return q.toPointF();
    }

    void drawLine(const QVector3D& a, const QVector3D& b) {
        QPointF t = project(a);
        QPointF u = project(b);
        painter.drawLine(t, u);
    }

    void drawTriangle(const QVector3D& a, const QVector3D& b, const QVector3D& c) {
        drawLine(a, b);
        drawLine(b, c);
        drawLine(c, a);
    }

    void drawPoint(const QVector3D& a, float r) {
        painter.drawEllipse(project(a), r, r);
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    static float a = 0;
    QMatrix4x4 rotator;
    rotator.rotate(a, {0, 1, 0});
    QVector3D camera_position = {0, 1, -2};
    camera_position = rotator * camera_position;
    a += 0.005;
    DDD ddd(painter, *this, camera_position, {0, 0, 0});
    painter.setBrush(Qt::black);

    QVector<QVector3D> cube = {
        QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0), QVector3D(1.0, 1.0, 0.0),
        QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 1.0, 0.0), QVector3D(1.0, 0.0, 0.0),
        QVector3D(1.0, 0.0, 0.0), QVector3D(1.0, 1.0, 0.0), QVector3D(1.0, 1.0, 1.0),
        QVector3D(1.0, 0.0, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, 0.0, 1.0),
        QVector3D(1.0, 0.0, 1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 1.0, 1.0),
        QVector3D(1.0, 0.0, 1.0), QVector3D(0.0, 1.0, 1.0), QVector3D(0.0, 0.0, 1.0),
        QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 1.0, 1.0), QVector3D(0.0, 1.0, 0.0),
        QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 1.0, 0.0), QVector3D(0.0, 0.0, 0.0),
        QVector3D(0.0, 1.0, 0.0), QVector3D(0.0, 1.0, 1.0), QVector3D(1.0, 1.0, 1.0),
        QVector3D(0.0, 1.0, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, 1.0, 0.0),
        QVector3D(1.0, 0.0, 1.0), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0),
        QVector3D(1.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0),
    };

    for (int i = 0; i < cube.size(); i += 3) {
        ddd.drawTriangle(cube[i], cube[i + 1], cube[i + 2]);
        ddd.drawPoint({1, 1, 1}, 5);
    }

    update();
}
