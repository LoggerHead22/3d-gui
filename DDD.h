
#ifndef DDD_H
#define DDD_H

#include <QPainter>
#include <QVector3D>
#include <QMatrix4x4>
#include <QWidget>
#include "triangle.h"

struct DDD {
    QPainter& painter;
    const float zNear;
    const float zFar;
    const float fov;
    const float width, height;
    const float a;
    const QVector3D eye, center;
    QMatrix4x4 transform;
    const QVector3D light;

    DDD(QPainter& painter, const QWidget& widget, float zNear, float zFar, float fov, const QVector3D& eye, const QVector3D& center, const QVector3D& light)
        : painter(painter)
        , zNear(zNear)
        , zFar(zFar)
        , fov(fov)
        , width(widget.size().width())
        , height(widget.size().height())
        , a(width / height)
        , eye(eye)
        , center(center)
        , light(-light)
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

    DDD(QPainter& painter, const QWidget& widget, const QVector3D& eye)
        : DDD(painter, widget, 0.1, 1000, 70, eye, {0, 0, 0}, {0, -1, 0})
    {}

    // проецирует точку из нормированной системы координат (НСК) в систему координат painter'a
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

    void fillTriangle(const QVector3D& a, const QVector3D& b, const QVector3D& c) {
        QPainterPath path;
        QVector<QPointF> poly = {project(a), project(b), project(c)};
        path.addPolygon(poly);
        painter.drawPath(path);
    }

    void fillPolygon(const QVector<QVector3D>& points, const QColor& color = Qt::black) {
        QPainterPath path;
        QVector<QPointF> poly;
        for (auto& it: points) {
            poly.push_back(project(it));
        }
        path.addPolygon(poly);
        painter.fillPath(path, QBrush(color));
    }

    void drawPoint(const QVector3D& a, float r) {
        painter.drawEllipse(project(a), r, r);
    }

    void drawText(const QVector3D& a, const QString& text) {
        painter.drawText(project(a), text);
    }

    // рисует оси, числа на осях, сетку
    //     size: размер графика в НСК
    //     *Range: диапазон значений по оси *
    //     n_*_points: количество точек на оси *
    void drawAxes(const QVector3D& size,
                  const QPair<double, double>& xRange, int n_x_points,
                  const QPair<double, double>& yRange, int n_y_points,
                  const QPair<double, double>& zRange, int n_z_points)
    {
        QVector3D ex = {size.x(), 0, 0};
        const QVector3D ey = {0, size.y(), 0};
        const QVector3D ez = {0, 0, size.z()};

        const QVector3D a = -size / 2;
        const QVector3D b = a + ez;
        const QVector3D c = b + ex;
        const QVector3D d = c - ez;
        const QVector3D e = a + ey;
        const QVector3D f = b + ey;
        const QVector3D g = c + ey;
        const QVector3D h = d + ey;

        const double xDiff = xRange.second - xRange.first;
        const double yDiff = yRange.second - yRange.first;
        const double zDiff = zRange.second - zRange.first;

        // переводит точку из системы координат графика в НСК
        auto toNdc = [&](const QVector3D& p) {
            float x = a.x() + (d.x() - a.x()) * ((p.x() - xRange.first) / xDiff);
            float y = a.y() + (e.y() - a.y()) * ((p.y() - yRange.first) / yDiff);
            float z = a.z() + (b.z() - a.z()) * ((p.z() - zRange.first) / zDiff);
            return QVector3D(x, y, z);
        };

        // границы графика
        drawLine(a, b);
        drawLine(b, c);
        drawLine(c, d);
        drawLine(d, a);

        drawLine(a, e);
        drawLine(b, f);
        drawLine(c, g);

        // отступы в НСК для текста (чтобы буковки не наезжали на оси)
        const QVector3D xTextOffset(0, 0, -0.2);
        const QVector3D yTextOffset(-0.2, 0, -0.2);
        const QVector3D zTextOffset(0.2, 0, 0);

        // точки на Ox & часть сетки
        for (double i = 0, x = xRange.first; i < n_x_points; i++, x += xDiff / n_x_points) {
            QVector3D begin(x, yRange.first, zRange.first);
            QVector3D end(x, yRange.first, zRange.second);
            drawText(toNdc(begin) + xTextOffset, QString::number(x));
            painter.setPen(QPen(Qt::DotLine));
            drawLine(toNdc(begin), toNdc(end));
        }

        // точки на Oy
        for (double i = 0, y = yRange.first; i < n_y_points; i++, y += yDiff / n_y_points) {
            QVector3D begin(xRange.first, y, zRange.first);
            drawText(toNdc(begin) + yTextOffset, QString::number(y));
        }

        // точки на Oz & часть сетки
        for (double i = 0, z = zRange.first; i < n_z_points; i++, z += zDiff / n_z_points) {
            QVector3D begin(xRange.second, yRange.first, z);
            QVector3D end(xRange.first, yRange.first, z);
            drawText(toNdc(begin) + zTextOffset, QString::number(z));
            painter.setPen(QPen(Qt::DotLine));
            drawLine(toNdc(begin), toNdc(end));
        }

        // отрисовка Ox, если требуется
        if (xRange.first * xRange.second <= 0) {
            painter.setPen(Qt::red);
            QVector3D begin(0, yRange.first, zRange.first);
            QVector3D end(0, yRange.first, zRange.second);
            drawLine(toNdc(begin), toNdc(end));
        }
        // отрисовка Oz, если требуется
        if (zRange.first * zRange.second <= 0) {
            painter.setPen(Qt::red);
            QVector3D begin(xRange.first, yRange.first, 0);
            QVector3D end(xRange.second, yRange.first, 0);
            drawLine(toNdc(begin), toNdc(end));
        }
        painter.setPen(Qt::black);
    }

    void drawTriangles(const std::vector<Triangle>& triangles, QColor l, QColor d) {
        for (const auto& it : triangles) {
            const QVector3D& p0 = it.p0;
            const QVector3D& p1 = it.p1;
            const QVector3D& p2 = it.p2;

            QVector3D normal = QVector3D::crossProduct(p1 - p0, p2 - p0);
            normal.normalize();

            float intensity = QVector3D::dotProduct(normal, light);
            intensity = qMax(0.f, intensity);
            int r = d.red() + (l.red() - d.red()) * intensity;
            int g = d.green() + (l.green() - d.green()) * intensity;
            int b = d.blue() + (l.blue() - d.blue()) * intensity;
            int a = d.alpha() + (l.alpha() - d.alpha()) * intensity;
            r = qMin(qMax(0, r), 255);
            g = qMin(qMax(0, g), 255);
            b = qMin(qMax(0, b), 255);
            a = qMin(qMax(0, a), 255);
            painter.setBrush(QColor(r, g, b, a));
            fillTriangle(p0, p1, p2);
        }
        painter.setBrush(Qt::black);
    }
};


#endif // DDD_H
