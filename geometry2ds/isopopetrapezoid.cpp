#include "isopopetrapezoid.h"

IsopopeTrapezoid::IsopopeTrapezoid(Resource * res)
    : Trapezoid(res)
{
}

IsopopeTrapezoid::IsopopeTrapezoid(QPointF const & pt)
    : Trapezoid(pt)
{
}

IsopopeTrapezoid::IsopopeTrapezoid(IsopopeTrapezoid const & o)
    : Trapezoid(o)
{
}

QPointF IsopopeTrapezoid::point(int index)
{
    if (index < 3)
        return Trapezoid::point(index);
    QPointF const & st = points_.first();
    QPointF const & pt = points_[1];
    QPointF const & pt2 = point(1);
    return QPointF(st.x() + pt2.x() - pt.x(), pt.y());
}

bool IsopopeTrapezoid::move(int elem, const QPointF &pt)
{
    if (elem == 0 || elem == 1) {
        Trapezoid::move(5, pt);
        return Polygon::move(elem, pt);
    } else if (elem == 2 || elem == 3) {
        Trapezoid::move(7, pt);
        return Polygon::move(elem, pt);
    } else {
        return Trapezoid::move(elem, pt);
    }
}
