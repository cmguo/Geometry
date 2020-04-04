#include "righttrapezoid.h"

RightTrapezoid::RightTrapezoid(Resource * res)
    : Trapezoid(res)
{
}

RightTrapezoid::RightTrapezoid(RightTrapezoid const & o)
    : Trapezoid(o)
{
}

QPointF RightTrapezoid::point4(QPointF const & pt1, QPointF const & pt2, QPointF const & pt3)
{
    (void) pt2;
    return QPointF(pt1.x(), pt3.y());
}
