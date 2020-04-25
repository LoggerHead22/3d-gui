#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>

struct Triangle {
    QVector3D p0, p1, p2;

    inline Triangle(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2)
        : p0(p0)
        , p1(p1)
        , p2(p2)
    {}

    inline QVector3D center() const {
        return (p0 + p1 + p2) / 3;
    }
};

#endif // TRIANGLE_H
