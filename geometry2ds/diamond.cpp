#include "diamond.h"

Diamond::Diamond(Resource * res)
    : Polygon(res)
{
}

Diamond::Diamond(QPointF const & pt)
    : Polygon(pt)
{
}

Diamond::Diamond(Diamond const & o)
    : Polygon(o)
{
}

int Diamond::pointCount()
{
    return points_.size() >= 2 ? 4 : 1;
}

QPointF Diamond::point(int index)
{
    QPointF const & pt1 = points_[0];
    QPointF const & pt2 = points_[1];
    QPointF c = (pt1 + pt2) / 2;
    switch (index) {
    case 0:
        return {c.x(), pt1.y()};
    case 1:
        return {pt2.x(), c.y()};
    case 2:
        return {c.x(), pt2.y()};
    case 3:
        return {pt1.x(), c.y()};
    }
    return Polygon::point(index);
}

bool Diamond::setPoint(int index, const QPointF &pt)
{
    QPointF & pt1 = points_[0];
    QPointF & pt2 = points_[1];
    QPointF c = pt1 + pt2;
    switch (index) {
    case 0:
        pt1.setY(pt.y());
        pt2.setY(c.y() - pt.y());
        return true;
    case 1:
        pt1.setX(c.x() - pt.x());
        pt2.setX(pt.x());
        return true;
    case 2:
        pt1.setY(c.y() - pt.y());
        pt2.setY(pt.y());
        return true;
    case 3:
        pt1.setX(pt.x());
        pt2.setX(c.x() - pt.x());
        return true;
    default:
        return false;
    }
}
