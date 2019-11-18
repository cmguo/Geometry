#include "righttriangle.h"

RightTriangle::RightTriangle(Resource * res)
    : Triangle(res)
{
}

RightTriangle::RightTriangle(QPointF const & pt)
    : Triangle(pt)
{
}

RightTriangle::RightTriangle(RightTriangle const & o)
    : Triangle(o)
{
}

QPointF RightTriangle::point(int index)
{
    if (index < 2) {
        return points_[index];
    }
    return QPointF(points_.first().x(), points_[1].y());
}

bool RightTriangle::move(int elem, const QPointF &pt)
{
    if (elem == 0) {
        Triangle::move(3, pt);
        return Triangle::move(0, pt);
    } else if (elem == 1) {
        Triangle::move(5, pt);
        return Triangle::move(1, pt);
    } else if (elem == 2) {
        return moveKeepAngle(elem, pt);
    } else {
        return Triangle::move(elem, pt);
    }
}
