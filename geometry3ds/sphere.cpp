#include "sphere.h"

#include <geometryhelper.h>

Sphere::Sphere(Resource * res)
    : Ellipsoid(res)
{
}

Sphere::Sphere(const Sphere &o)
    : Ellipsoid(o)
{
}

void Sphere::sync()
{
    if (dirty_ && points_.size() > 1) {
        qreal r = GeometryHelper::length(points_.back() - points_.first());
        radius_.setX(float(r));
        radius_.setY(float(r));
        radius_.setZ(float(r));
        dirty_ = false;
    }
}

QVector<QPointF> Sphere::movePoints()
{
    QVector<QPointF> points;
    QPointF c(points_.front());
    QPointF off(points_.back() - c);
    points.append(points_.back());
    off = QPointF(-off.y(), off.x());
    points.append(c + off);
    off = QPointF(-off.y(), off.x());
    points.append(c + off);
    off = QPointF(-off.y(), off.x());
    points.append(c + off);
    return points;
}

int Sphere::hit(QPointF &pt)
{
    QPointF off(points_.back() - points_.front());
    qreal prod = QPointF::dotProduct(off, off);
    QPointF off2(pt - points_.front());
    qreal prod2 = QPointF::dotProduct(off2, off2);
    off = off2 * sqrt(prod / prod2);
    off2 -= off;
    if (GeometryHelper::length2(off2) < GeometryHelper::HIT_DIFF_DIFF) {
        pt = points_.front() + off;
        return 1;
    }
    return -1;
}

bool Sphere::moveElememt(int elem, QPointF const & pt)
{
    if (elem < 4)
        return Geometry3D::moveElememt(1, pt);
    return false;
}
