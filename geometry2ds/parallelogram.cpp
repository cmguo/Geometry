#include "parallelogram.h"
#include "base/geometryhelper.h"

Parallelogram::Parallelogram(Resource * res)
    : Trapezoid(res)
{
}

Parallelogram::Parallelogram(Parallelogram const & o)
    : Trapezoid(o)
{
}

Parallelogram::Parallelogram(const QPointF &vec1, const QPointF &vec2, const QPointF &vec3)
    : Trapezoid(vec1, vec2, vec3)
{
}

QPointF Parallelogram::point4(QPointF const & pt1, QPointF const & pt2, QPointF const & pt3)
{
    return pt1 + pt3 - pt2;
}

bool Parallelogram::setPoint(int index, const QPointF &pt)
{
    if (index == 0) {
        QPointF pt2 = point(2);
        QPointF pt3 = point(3);
        Trapezoid::setPoint(0, pt);
        Trapezoid::setPoint(1, QPointF(pt.x() + pt2.x() - pt3.x(), pt.y()));
        return true;
    }
    if (index < 3) {
        return Trapezoid::setPoint(index, pt);
    }
    QPointF st = point(0);
    QPointF pt1 = point(1);
    return Trapezoid::setPoint(2, QPointF(pt1.x() + pt.x() - st.x(), pt.y()));
}

bool Parallelogram::moveElememt(int elem, const QPointF &pt)
{
    if (elem == 0) {
        QPointF p = pt;
        GeometryHelper::attachToLines(point(3), p);
        return Polygon::moveElememt(elem, p);
    } else if (elem == 3) {
        QPointF p = pt;
        GeometryHelper::attachToLines(point(0), p);
        return Polygon::moveElememt(elem, p);
    } else {
        return Trapezoid::moveElememt(elem, pt);
    }
}
