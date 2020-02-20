#include "arbitrarypolygon.h"
#include "geometryhelper.h"

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

void ArbitraryPolygon::addPoint(QPointF const & pt)
{
    Polygon::addPoint(pt);
    tempValid_ = false;
}

void ArbitraryPolygon::movePoint(QPointF const & pt)
{
    qDebug() << "movePoint";
    if (!points_.isEmpty())
        points_.back() = pt;
}

bool ArbitraryPolygon::commitPoint(const QPointF &pt)
{
    tempValid_ = false;
    if (pointCount() < 4)
        return false;
    QPointF d = pt - points_.first();
    if (GeometryHelper::length2(d) >= GeometryHelper::HIT_DIFF_DIFF)
        return false;
    points_.pop_back();
    return true;
}

bool ArbitraryPolygon::moveTempPoint(const QPointF &pt)
{
    qDebug() << "moveTempPoint";
    temp_ = pt;
    tempValid_ = true;
    dirty_ = true;
    return true;
}

bool ArbitraryPolygon::canFinish()
{
    return points_.size() > 2;
}

void ArbitraryPolygon::finish(const QPointF &c)
{
    if (tempValid_) {
        tempValid_ = false;
        dirty_ = true;
        QPointF c1 = path().boundingRect().center();
        Polygon::finish(c1);
    } else {
        Polygon::finish(c);
    }
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
