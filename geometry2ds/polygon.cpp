#include "polygon.h"
#include "base/geometryhelper.h"
#include "regularpolygon.h"

#include <core/optiontoolbuttons.h>

#include <QPainter>

static WidthToolButtons buttonRadius({0, 2.0, 4.0, 8.0, 16.0, 32.0});
REGISTER_OPTION_BUTTONS(Polygon, radius, buttonRadius);

Polygon::Polygon(Resource * res, Flags flags)
    : Geometry2D(res, flags)
    , radius_(0)
{
}

Polygon::Polygon(Polygon const & o)
    : Geometry2D(o)
    , graphPath_(o.graphPath_)
    , textPath_(o.textPath_)
    , radius_(o.radius_)
{
}

Polygon::Polygon(const QPolygonF &polygon)
    : Geometry2D(polygon.mid(0, polygon.size() - 1))
    , radius_(0)
{
}

Polygon::Polygon(const QVector<QPointF> &points)
    : Geometry2D(points)
    , radius_(0)
{
}

QPainterPath Polygon::graphPath()
{
    return graphPath_;
}

QPainterPath Polygon::textPath()
{
    return textPath_;
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
    if (pointCount() > 2) {
        if (canClose()) {
            points[pointCount()] = (pt0 + first) / 2.0;
        } else {
            points.removeAt(pointCount());
        }
    } else {
        points.resize(pointCount());
    }
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
    if (!canClose()) {
        if (line == pointCount())
            line = -1;
        else
            line -= 1;
    }
    return line;
}

bool Polygon::moveElememt(int elem, const QPointF &pt)
{
    if (elem < pointCount()) {
        return setPoint(elem, pt);
    }
    elem -= pointCount();
    if (elem >= pointCount())
        return false;
    if (!canClose())
        elem += 1;
    QPointF hint;
    QPointF llpt = iterPoint((pointCount() + elem - 2) % pointCount(), hint);
    QPointF lpt = nextPoint((pointCount() + elem - 1) % pointCount(), hint);
    QPointF npt = nextPoint(elem, hint);
    QPointF nnpt = nextPoint((elem + 1) % pointCount(), hint);
    if (!canClose()) {
        if (elem == 1) {
            llpt = lpt + nnpt - npt;
        } else if (elem == pointCount() - 1) {
            nnpt = npt + llpt - lpt;
        }
    }
    moveLine(llpt, lpt, pt, npt, nnpt);
    setPoint((pointCount() + elem - 1) % pointCount(), lpt);
    setPoint(elem, npt);
    return true;
}

void Polygon::sync()
{
    if (!dirty_)
        return;
    dirty_ = false;
    int pointCount = this->pointCount();
    if (pointCount < 2)
        return;
    bool canClose = this->canClose();
    QPainterPath ph;
    QPainterPath tph;
    QPolygonF polygon;
    QPointF hint;
    QPointF first = firstPoint(hint);
    polygon.append(first);
    for (int i = 1; i < pointCount; ++i)
        polygon.append(nextPoint(i, hint));
    if (canClose) {
        polygon.append(first);
    }
    if (qFuzzyIsNull(radius_))
        ph.addPolygon(polygon);
    else
        ph = GeometryHelper::toRoundPolygon(polygon, radius_);
    if (pointCount > 2 && qFuzzyIsNull(radius_)) {
        QPointF lpt = lastPoint(hint);
        QPointF cpt = nextPoint(0, hint);
        int i = 0;
        if (!canClose) {
            lpt = cpt;
            cpt = nextPoint(++i, hint);
        }
        for (; i < pointCount - 1; ++i) {
            QPointF npt = nextPoint(i + 1, hint);
            addAngleLabeling(ph, tph, lpt, cpt, npt);
            lpt = cpt;
            cpt = npt;
        }
        if (canClose)
            addAngleLabeling(ph, tph, lpt, cpt, first);
    }
    graphPath_ = ph;
    textPath_ = tph;
}

bool Polygon::canClose()
{
    return true;
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
