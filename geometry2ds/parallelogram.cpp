#include "parallelogram.h"

Parallelogram::Parallelogram(Resource * res)
    : Trapezoid(res)
{
}

Parallelogram::Parallelogram(QPointF const & pt)
    : Trapezoid(pt)
{
}

Parallelogram::Parallelogram(Parallelogram const & o)
    : Trapezoid(o)
{
}

QPointF Parallelogram::point(int index)
{
    if (index < 3)
        return Trapezoid::point(index);
    QPointF const & st = points_.first();
    QPointF const & pt1 = point(1);
    QPointF const & pt2 = points_[1];
    return QPointF(st.x() + pt2.x() - pt1.x(), pt2.y());
}

bool Parallelogram::setPoint(int index, const QPointF &pt)
{
    if (index < 3)
        return Trapezoid::setPoint(index, pt);
    QPointF const & st = points_.first();
    QPointF const & pt1 = point(1);
    QPointF const & pt2 = points_[1];
    return Trapezoid::setPoint(1, QPointF(st.x() + pt2.x() - pt.x(), pt1.y()));
}

bool Parallelogram::move(int elem, const QPointF &pt)
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
