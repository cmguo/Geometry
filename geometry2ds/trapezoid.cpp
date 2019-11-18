#include "trapezoid.h"

Trapezoid::Trapezoid(Resource * res)
    : Polygon(res)
{
}

Trapezoid::Trapezoid(QPointF const & pt)
    : Polygon(pt)
{
}

Trapezoid::Trapezoid(Trapezoid const & o)
    : Polygon(o)
{
}

int Trapezoid::pointCount()
{
    return points_.size() >= 2 ? 4 : 1;
}

bool Trapezoid::setPoint(int index, const QPointF &pt)
{
    if ((index & 1) == 0) {
        points_[index >> 1] = pt;
    } else if (index == 1) {
        if (points_.size() == 3)
            points_[2] = pt;
        else
            points_.append(pt);
    }
    return true;
}

QPointF Trapezoid::point(int index)
{
    if ((index & 1) == 0)
        return points_[index >> 1];
    if (index == 1 && points_.size() == 3)
        return points_[2];
    QPointF const & st = points_.first();
    QPointF const & pt = points_[1];
    if (index == 1) {
        return QPointF((st.x() + pt.x()) / 2, st.y());
    } else {
        return QPointF(); // implemention defined
    }
}

bool Trapezoid::move(int elem, const QPointF &pt)
{
    if (elem < 4)
        return moveKeepAngle(elem, pt);
    return Polygon::move(elem, pt);
}
