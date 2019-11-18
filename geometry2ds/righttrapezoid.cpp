#include "righttrapezoid.h"

RightTrapezoid::RightTrapezoid(Resource * res)
    : Trapezoid(res)
{
}

RightTrapezoid::RightTrapezoid(QPointF const & pt)
    : Trapezoid(pt)
{
}

RightTrapezoid::RightTrapezoid(RightTrapezoid const & o)
    : Trapezoid(o)
{
}

QPointF RightTrapezoid::point(int index)
{
    if (index < 3)
        return Trapezoid::point(index);
    QPointF const & st = points_.first();
    QPointF const & pt = points_[1];
    return QPointF(st.x(), pt.y());
}

bool RightTrapezoid::move(int elem, const QPointF &pt)
{
    if (elem == 1) {
        Trapezoid::move(5, pt);
        return Polygon::move(1, pt);
    } else if (elem == 2) {
        Trapezoid::move(7, pt);
        return Polygon::move(2, pt);
    } else {
        return Trapezoid::move(elem, pt);
    }
}
