#include "righttrapezoid.h"

RightTrapezoid::RightTrapezoid(Resource * res)
    : Trapezoid(res)
{
}

RightTrapezoid::RightTrapezoid(RightTrapezoid const & o)
    : Trapezoid(o)
{
}

RightTrapezoid::RightTrapezoid(const QPointF &vec1, const QPointF &vec2, const QPointF &vec3)
    : Trapezoid(vec1, vec2, vec3)
{
}

QPointF RightTrapezoid::point4(QPointF const & pt1, QPointF const & pt2, QPointF const & pt3)
{
    (void) pt2;
    return QPointF(pt1.x(), pt3.y());
}
