#include "rectangle.h"

Rectangle::Rectangle(Resource * res)
    : Polygon(res)
{
}

Rectangle::Rectangle(Rectangle const & o)
    : Polygon(o)
{
}

int Rectangle::pointCount()
{
    return points_.size() == 2 ? 4 : 1;
}

bool Rectangle::setPoint(int index, const QPointF &pt)
{
    if ((index & 1) == 0) {
        Polygon::setPoint(index >> 1, pt);
    }
    return true;
}

QPointF Rectangle::point(int index)
{
    if ((index & 1) == 0) {
        return points_[index >> 1];
    }
    QPointF const & st = points_.first();
    QPointF const & pt = points_[1];
    if (index == 1) {
        return QPointF(pt.x(), st.y());
    } else {
        return QPointF(st.x(), pt.y());
    }
}

bool Rectangle::moveElememt(int elem, const QPointF &pt)
{
    if (elem < 4)
        return moveKeepAngle(elem, pt);
    return Polygon::moveElememt(elem, pt);
}
