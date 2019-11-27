#include "triangle.h"

Triangle::Triangle(Resource * res)
    : Polygon(res, DrawAttach)
{
}

Triangle::Triangle(QPointF const & pt)
    : Polygon(pt)
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
        points_[index] = pt;
    }
    return true;
}

bool Triangle::move(int elem, const QPointF &pt)
{
    bool result = Polygon::move(elem, pt);
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
