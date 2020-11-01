#include "righttriangle.h"
#include "geometryhelper.h"

RightTriangle::RightTriangle(Resource * res)
    : Triangle(res)
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

bool RightTriangle::moveElememt(int elem, const QPointF &pt)
{
    if (elem == 0) {
        QPointF p(pt);
        GeometryHelper::attachToLines(points_[1], p);
        Triangle::moveElememt(3, p);
        return Triangle::moveElememt(0, p);
    } else if (elem == 1) {
        QPointF p(pt);
        GeometryHelper::attachToLines(points_[0], p);
        Triangle::moveElememt(5, p);
        return Triangle::moveElememt(1, p);
    } else if (elem == 2) {
        return moveKeepAngle(elem, pt);
    } else {
        return Triangle::moveElememt(elem, pt);
    }
}
