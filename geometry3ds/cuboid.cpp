#include "cuboid.h"

Cuboid::Cuboid(Resource * res)
    : Polyhedron(res)
{
    makeLine(0, 1);
    makeLine(1, 2);
    makeLine(2, 3);
    makeLine(3, 0);
    makeLine(0, 6);
    makeLine(6, 5);
    makeLine(5, 4);
    makeLine(4, 7);
    makeLine(7, 6);
    makeLine(7, 3);
    makeLine(4, 2);
    makeLine(5, 1);
}

Cuboid::Cuboid(Cuboid const & o)
    : Polyhedron(o)
    , moveElem_(o.moveElem_)
    , inner_(o.inner_)
{
}

int Cuboid::pointCount()
{
    return points_.size() >= 2 ? 8 : 1;
}

// @see Geometry3D::PO,Geometry3D::PI

QVector3D Cuboid::point(int index)
{
    if (dirty_ && points_.size() > 1) {
        QPointF pt1 = points_[0];
        QPointF pt2 = points_[1];
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        qreal x0, y0, z0;
        qreal w, h;
        if (inner_) {
            x0 = pt1.x();
            y0 = 0;
            h = (pt2.y() - pt1.y()) / (1 - CO);
            z0 = -pt1.y() - h;
            w = pt2.x() - pt1.x() - h * CO;
        } else {
            y0 = 0;
            z0 = -pt2.y();
            h = (pt2.y() - pt1.y()) / (1 + CO);
            x0 = pt1.x() - CO * h;
            w = pt2.x() - x0;
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
    case 6:
        break;
    case 1:
    case 5:
        pt.setX(pt.x() + size_.x()); break;
    case 2:
    case 4:
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

void Cuboid::setMoveElem(int elem)
{
    if (moveElem_ != elem) {
        int fix = (8 - elem);
        if ((fix % 4) == 0) fix -= 4;
        points_[0] = PO.map(point(fix)).toPointF();
        moveElem_ = elem;
        inner_ = QVector<int>({2,3,5,6}).contains(elem);
    }
}

bool Cuboid::move(int elem, const QPointF &pt)
{
    setMoveElem(elem);
    return Polyhedron::move(1, pt);
}
