#include "cube.h"

Cube::Cube(Resource * res)
    : Cuboid(res)
{
}

Cube::Cube(Cube const & o)
    : Cuboid(o)
{
}

void Cube::movePoint(const QPointF &pt)
{
    if (points_.size() == 1) {
        points_.append(pt);
    }
    move(1, pt);
}

bool Cube::move(int elem, const QPointF &pt)
{
    setMoveElem(elem);
    points_[1] = pt;
    QPointF & pt1 = points_[0];
    QPointF & pt2 = points_[1];
    if (inner_) {
        qreal h = (pt2.y() - pt1.y()) / (1 - CO);
        qreal w = pt2.x() - pt1.x() - h * CO;
        if (qAbs(w) < qAbs(h)) {
            w = w > 0 ? qAbs(h) : -qAbs(h);
        } else {
            h = h > 0 ? qAbs(w) : -qAbs(w);
        }
        pt2.setX(pt1.x() + CO * h + w);
        pt2.setY(h * (1 - CO) + pt1.y());
    } else {
        qreal h = (pt2.y() - pt1.y()) / (1 + CO);
        qreal x0 = pt1.x() - CO * h;
        qreal w = pt2.x() - x0;
        if (qAbs(w) < qAbs(h)) {
            w = w > 0 ? qAbs(h) : -qAbs(h);
        } else {
            h = h > 0 ? qAbs(w) : -qAbs(w);
        }
        pt2.setX(pt1.x() - CO * h + w);
        pt2.setY(h * (1 + CO) + pt1.y());
    }
    dirty_ = true;
    return true;
}
