#include "triangle.h"

Triangle::Triangle(Resource * res)
    : Polygon(res, DrawAttach)
{
}

Triangle::Triangle(Triangle const & o)
    : Polygon(o)
{
}

int Triangle::pointCount()
{
    return points_.size() == 2 ? 3 : 1;
}

bool Triangle::setPoint(int index, const QPointF &pt)
{
    if (index < 2) {
        Polygon::setPoint(index, pt);
    }
    return true;
}

bool Triangle::moveElememt(int elem, const QPointF &pt)
{
    bool result = Polygon::moveElememt(elem, pt);
    /*
    if (result && elem < 3) {
        QPointF lpt = points_.back();
        for (int i = 0; i < 3; ++i) {
            QPointF pt = points_[i];
            QPointF npt = i == 2 ? points_.front() : points_[i + 1];
            angles_[i] = angle(lpt, pt, npt);
            lpt = pt;
        }
    }
    */
    return result;
}
