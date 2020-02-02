#include "polygon.h"
#include "arbitrarypolygon.h"
#include "geometryhelper.h"

#include <QPainter>

Polygon::Polygon(Resource * res, Flags flags)
    : Geometry2D(res, flags)
{
}

Polygon::Polygon(QPointF const & pt)
    : Geometry2D(pt)
{
}

Polygon::Polygon(Polygon const & o)
    : Geometry2D(o)
    , path_(o.path_)
    , textPath_(o.textPath_)
{
}

QPainterPath Polygon::path()
{
    if (!dirty_)
        return path_ | textPath_;
    dirty_ = false;
    QPainterPath ph;
    QPainterPath tph;
    if (pointCount() < 2)
        return ph;
    QPolygonF polygon;
    QPointF hint;
    QPointF first = firstPoint(hint);
    polygon.append(first);
    for (int i = 1; i < pointCount(); ++i)
        polygon.append(nextPoint(i, hint));
    if (metaObject() != &ArbitraryPolygon::staticMetaObject
            || (flags_ & DrawFinised)) {
        polygon.append(first);
    }
    ph.addPolygon(polygon);
    if (pointCount() > 2) {
        QPointF lpt = lastPoint(hint);
        QPointF cpt = nextPoint(0, hint);
        for (int i = 0; i < pointCount() - 1; ++i) {
            QPointF npt = nextPoint(i + 1, hint);
            addAngleLabeling(ph, tph, lpt, cpt, npt);
            lpt = cpt;
            cpt = npt;
        }
        addAngleLabeling(ph, tph, lpt, cpt, first);
    }
    path_ = ph;
    textPath_ = tph;
    return path_ | textPath_;
}

QVector<QPointF> Polygon::movePoints()
{
    QVector<QPointF> points(pointCount() * 2);
    QPointF hint;
    QPointF first = firstPoint(hint);
    QPointF pt0 = first;
    points[0] = pt0;
    for (int i = 1; i < pointCount(); ++i) {
        QPointF pt(nextPoint(i, hint));
        points[i] = pt;
        points[pointCount() + i] = (pt0 + pt) / 2.0;
        pt0 = pt;
    }
    points[pointCount()] = (pt0 + first) / 2.0;
    return points;
}

int Polygon::hit(QPointF & pt)
{
    QPointF hint;
    QPointF rp;
    QPointF lpt = lastPoint(hint);
    int line = -1;
    for (int i = 0; i < pointCount(); ++i) {
        QPointF cpt = nextPoint(i, hint);
        QPointF d = pt - cpt;
        if (GeometryHelper::length2(d) < GeometryHelper::HIT_DIFF_DIFF) {
            pt = cpt;
            return i;
        }
        if (line == -1 && GeometryHelper::dist2PointToSegment(lpt, cpt, pt, rp) < GeometryHelper::HIT_DIFF_DIFF) {
            pt = rp;
            line = i + pointCount();
        }
        lpt = cpt;
    }
    return line;
}

bool Polygon::move(int elem, const QPointF &pt)
{
    if (elem < pointCount()) {
        return setPoint(elem, pt);
    }
    elem -= pointCount();
    if (elem >= pointCount())
        return false;
    QPointF hint;
    QPointF llpt = iterPoint((pointCount() + elem - 2) % pointCount(), hint);
    QPointF lpt = nextPoint((pointCount() + elem - 1) % pointCount(), hint);
    QPointF npt = nextPoint(elem, hint);
    QPointF nnpt = nextPoint((elem + 1) % pointCount(), hint);
    moveLine(llpt, lpt, pt, npt, nnpt);
    setPoint((pointCount() + elem - 1) % pointCount(), lpt);
    setPoint(elem, npt);
    return true;
}

void Polygon::draw(QPainter *painter)
{
    painter->setPen(QPen(color_, width_));
    painter->drawPath(path_);
    painter->save();
    painter->setPen(QPen(color_));
    painter->setBrush(color_);
    painter->drawPath(textPath_);
    painter->restore();
}

bool Polygon::moveKeepAngle(int elem, const QPointF &pt)
{
    if (elem >= pointCount())
        return false;
    QPointF hint;
    QPointF llpt = iterPoint((pointCount() + elem - 2) % pointCount(), hint);
    QPointF lpt = nextPoint((pointCount() + elem - 1) % pointCount(), hint);
    QPointF npt = nextPoint(elem, hint);
    QPointF nnpt = nextPoint((elem + 1) % pointCount(), hint);
    moveLine(llpt, lpt, pt, npt, nnpt);
    setPoint((pointCount() + elem - 1) % pointCount(), lpt);
    QPointF nnnpt = nextPoint((elem + 2) % pointCount(), hint);
    moveLine(lpt, npt, pt, nnpt, nnnpt);
    setPoint((pointCount() + elem + 1) % pointCount(), nnpt);
    setPoint(elem, pt);
    return true;
}

qreal Polygon::angle(int index)
{
    QPointF hint;
    QPointF lpt = iterPoint((pointCount() + index - 1) % pointCount(), hint);
    QPointF pt = nextPoint(index, hint);
    QPointF npt = nextPoint((index + 1) % pointCount(), hint);
    return GeometryHelper::angle(lpt, pt, npt);
}

void Polygon::addAngleLabeling(QPainterPath &path, QPainterPath &textPath, int index)
{
    QPointF hint;
    QPointF lpt = iterPoint((pointCount() + index - 1) % pointCount(), hint);
    QPointF pt = nextPoint(index, hint);
    QPointF npt = nextPoint((index + 1) % pointCount(), hint);
    addAngleLabeling(path, textPath, lpt, pt, npt);
}

int Polygon::pointCount()
{
    return points_.size();
}

QPointF Polygon::point(int index)
{
    return points_[index];
}

bool Polygon::setPoint(int index, const QPointF &pt)
{
    points_[index] = pt;
    dirty_ = true;
    return true;
}

QPointF Polygon::lastPoint(QPointF & hint)
{
    return iterPoint(pointCount() - 1, hint);
}

QPointF Polygon::firstPoint(QPointF & hint)
{
    return iterPoint(0, hint);
}

QPointF Polygon::iterPoint(int index, QPointF & hint)
{
    (void) hint;
    return point(index);
}

QPointF Polygon::prevPoint(int index, QPointF & hint)
{
    (void) hint;
    return point(index);
}

QPointF Polygon::nextPoint(int index, QPointF & hint)
{
    (void) hint;
    return point(index);
}

