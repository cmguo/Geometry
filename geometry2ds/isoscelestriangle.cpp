#include "isoscelestriangle.h"

IsoscelesTriangle::IsoscelesTriangle(Resource * res)
    : Triangle(res)
{
}

IsoscelesTriangle::IsoscelesTriangle(QPointF const & pt)
    : Triangle(pt)
{
}

IsoscelesTriangle::IsoscelesTriangle(IsoscelesTriangle const & o)
    : Triangle(o)
{
}

QPointF IsoscelesTriangle::point(int index)
{
    if (index < 2) {
        return points_[index];
    }
    return QPointF(points_.first().x() * 2 - points_[1].x(), points_[1].y());
}

bool IsoscelesTriangle::move(int elem, const QPointF &pt)
{
    if (elem == 0) {
        QPointF p(pt);
        p.setX(0);
        return Triangle::move(0, p);
    } else if (elem < 3) {
        return moveKeepAngle(elem, pt);
    } else {
        return Triangle::move(elem, pt);
    }
}
