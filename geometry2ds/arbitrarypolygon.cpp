#include "arbitrarypolygon.h"

ArbitraryPolygon::ArbitraryPolygon(Resource * res)
    : Polygon(res)
{
}

ArbitraryPolygon::ArbitraryPolygon(QPointF const & pt)
    : Polygon(pt)
{
}

ArbitraryPolygon::ArbitraryPolygon(ArbitraryPolygon const & o)
    : Polygon(o)
{
}

bool ArbitraryPolygon::commitPoint(const QPointF &pt)
{
    if (pointCount() < 4)
        return false;
    QPointF d = pt - points_.first();
    if (QPointF::dotProduct(d, d) >= HIT_DIFF_DIFF)
        return false;
    points_.pop_back();
    return true;
}

bool ArbitraryPolygon::moveTempPoint(const QPointF &pt)
{
    temp_ = pt;
    tempValid_ = true;
    return true;
}

bool ArbitraryPolygon::canFinish()
{
    return points_.size() > 2;
}

int ArbitraryPolygon::pointCount()
{
    return points_.size() + ((!tempValid_ || (flags_ & DrawFinised)) ? 0 : 1);
}

QPointF ArbitraryPolygon::point(int index)
{
    if (index < points_.size())
        return points_[index];
    else
        return temp_;
}
