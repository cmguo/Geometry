#include "diamond.h"

Diamond::Diamond(Resource * res)
    : Parallelogram(res)
{
}

Diamond::Diamond(QPointF const & pt)
    : Parallelogram(pt)
{
}

Diamond::Diamond(Diamond const & o)
    : Parallelogram(o)
{
}

QPointF Diamond::point(int index)
{
    if (index == 1) {
        QPointF const & st = points_.first();
        QPointF const & pt2 = points_[1];
        QPointF p1 = (st + pt2) / 2.0;
        QPointF p2 = pt2 - st;
        p2 = p1 + QPointF(-p2.y(), p2.x());
        QPointF st1(pt2.x(), st.y());
        return crossPoint(st, st1, p1, p2);
    }
    return Parallelogram::point(index);
}

bool Diamond::setPoint(int index, const QPointF &pt)
{
    if (index == 1) {
        return true;
    }
    return Parallelogram::setPoint(index, pt);
}
