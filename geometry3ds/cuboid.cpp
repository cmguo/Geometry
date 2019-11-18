#include "cuboid.h"

Cuboid::Cuboid(Resource * res)
    : Polyhedron(res)
{
    makeLine(0, 1);
    makeLine(1, 2);
    makeLine(2, 3);
    makeLine(3, 0);
    makeLine(0, 4);
    makeLine(4, 5);
    makeLine(5, 6);
    makeLine(6, 7);
    makeLine(7, 4);
    makeLine(7, 3);
    makeLine(6, 2);
    makeLine(5, 1);
}

Cuboid::Cuboid(QPointF const & pt)
    : Polyhedron(pt)
{
}

Cuboid::Cuboid(Cuboid const & o)
    : Polyhedron(o)
{
}

int Cuboid::pointCount()
{
    return points_.size() >= 2 ? 8 : 1;
}

QVector3D Cuboid::point(int index)
{
    if (dirty_ && points_.size() > 1) {
        QPointF pt1 = points_[0];
        QPointF pt2 = points_[1];
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        qreal y0 = 0;
        qreal z0 = -pt2.y();
        qreal h = (pt2.y() - pt1.y()) / (1 + CO);
        qreal x0 = pt1.x() - CO * h;
        qreal w = pt2.x() - x0;
        if (QByteArray("Cube") == metaObject()->className()) {
            qDebug() << "Cube";
            if (qAbs(w) < qAbs(h)) {
                w = w > 0 ? qAbs(h) : -qAbs(h);
            } else {
                h = h > 0 ? qAbs(w) : -qAbs(w);
            }
        }
#else
        qreal x0 = 0;
        qreal y0 = 0;
        qreal z0 = 0;
        qreal w = 300;
        qreal h = 200;
#endif
        origin_ = QVector3D(float(x0), float(y0), float(z0));
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        size_ = QVector3D(float(w), float(h), float(h));
#else
        size_ = QVector3D(float(h), float(w), float(h));
#endif
        dirty_ = false;
    }
    QVector3D pt(origin_);
    if (index >= 4)
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        pt.setY(size_.y());
#else
        pt.setZ(size_.z());
#endif
    switch (index) {
    case 0:
    case 4:
        break;
    case 1:
    case 5:
        pt.setX(pt.x() + size_.x()); break;
    case 2:
    case 6:
        pt.setX(pt.x() + size_.x());
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        pt.setZ(pt.z() + size_.z());
#else
        pt.setY(pt.y() + size_.y());
#endif
        break;
    case 3:
    case 7:
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        pt.setZ(pt.z() + size_.z());
#else
        pt.setY(pt.y() + size_.y());
#endif
        break;
    }
    return pt;
}

bool Cuboid::move(int elem, const QPointF &pt)
{
    return Polyhedron::move(elem, pt);
}
