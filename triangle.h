#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>
#include <QDebug>

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

inline QDebug operator<<(QDebug out, const Triangle& trio) {
    return out << "{" << trio.p0 << ", " << trio.p1 << ", " << trio.p2 << "}";
}

#endif // TRIANGLE_H
