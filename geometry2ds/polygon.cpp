#include "polygon.h"

#include <QtMath>
#include <QFont>

Polygon::Polygon(Resource * res)
    : Geometry2D(res)
{
}

Polygon::Polygon(QPointF const & pt)
    : Geometry2D(pt)
{
}

Polygon::Polygon(Polygon const & o)
    : Geometry2D(o)
{
}

bool Polygon::commit(const QPointF &pt)
{
    if (metaObject() != &Polygon::staticMetaObject) {
        return Geometry2D::commit(pt);
    }
    if (pointCount() < 4)
        return false;
    QPointF d = pt - points_.first();
    if (QPointF::dotProduct(d, d) >= HIT_DIFF_DIFF)
        return false;
    points_.pop_back();
    return true;
}

bool Polygon::canFinish()
{
    return pointCount() > 2;
}

QPainterPath Polygon::path()
{
    QPainterPath ph;
    if (pointCount() < 2)
        return ph;
    QPolygonF polygon;
    QPointF hint;
    QPointF first = firstPoint(hint);
    polygon.append(first);
    for (int i = 1; i < pointCount(); ++i)
        polygon.append(nextPoint(i, hint));
    if (metaObject() != &Polygon::staticMetaObject
            || (flags_ & DrawFinised)) {
        polygon.append(first);
    }
    ph.addPolygon(polygon);
    if (pointCount() > 2) {
        QPointF lpt = lastPoint(hint);
        QPointF cpt = nextPoint(0, hint);
        for (int i = 0; i < pointCount() - 1; ++i) {
            QPointF npt = nextPoint(i + 1, hint);
            addAngleLabeling(ph, lpt, cpt, npt);
            lpt = cpt;
            cpt = npt;
        }
        addAngleLabeling(ph, lpt, cpt, first);
    }
    return ph;
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
        if (QPointF::dotProduct(d, d) < HIT_DIFF_DIFF) {
            pt = cpt;
            return i;
        }
        if (line == -1 && dist2PointToSegment(lpt, cpt, pt, rp) < HIT_DIFF_DIFF) {
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
    return angle(lpt, pt, npt);
}

void Polygon::addAngleLabeling(QPainterPath &path, int index)
{
    QPointF hint;
    QPointF lpt = iterPoint((pointCount() + index - 1) % pointCount(), hint);
    QPointF pt = nextPoint(index, hint);
    QPointF npt = nextPoint((index + 1) % pointCount(), hint);
    addAngleLabeling(path, lpt, pt, npt);
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

/*
 * Static functions
 */

qreal Polygon::angle(QPointF const & p1, QPointF const & p2, QPointF const & p3)
{
    qreal dot = QPointF::dotProduct(p1 - p2, p3 - p2);
    qreal mod1 = QPointF::dotProduct(p1 - p2, p1 - p2);
    qreal mod2 = QPointF::dotProduct(p3 - p2, p3 - p2);
    return acos(dot / sqrt(mod1 * mod2)) * 180 / M_PI;
}

QPointF Polygon::crossPoint(QPointF const & p1, QPointF const & p2,
                          QPointF const & q1, QPointF const & q2)
{
    QPointF a(p1.y() - p2.y(), q1.y() - q2.y()); // a1, a2
    QPointF b(p2.x() - p1.x(), q2.x() - q1.x()); // b1, b2
    QPointF c(crossProduct(p1, p2), crossProduct(q1, q2));
    qreal d = crossProduct(a, b);
    return QPointF(crossProduct(b, c), crossProduct(c, a)) / d;
}

void Polygon::moveLine(QPointF const & llpt, QPointF & lpt, QPointF const & pt,
                QPointF & npt, QPointF const & nnpt)
{
    QPointF lp = crossPoint(llpt, lpt, pt, pt + npt - lpt);
    QPointF np = crossPoint(npt, nnpt, pt, pt + npt - lpt);
    lpt = lp;
    npt = np;
}

void Polygon::addAngleLabeling(QPainterPath &path, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt)
{
    addAngleLabeling(path, lpt, pt, npt, angle(lpt, pt, npt));
}

void Polygon::addAngleLabeling(QPainterPath &path, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt, qreal angle)
{
    QPointF lp = lpt;
    QPointF np = npt;
    adjustToLength(pt, lp, 10.0);
    adjustToLength(pt, np, 10.0);
    QPointF rpt = lp + np - pt;
    QPointF txt = rpt * 2 - pt + QPointF(-8, 6);
    //qDebug() << pt << angle;
    if (qFuzzyCompare(angle, 90.0)) {
        path.moveTo(lp);
        path.lineTo(rpt);
        path.lineTo(np);
        path.addText(txt, QFont(), QString("90°"));
    } else {
        QRectF bound(-14.14, -14.14, 28.28, 28.28);
        bound.moveCenter(pt);
        qreal r1 = Geometry2D::angle(lpt - pt);
        qreal r2 = Geometry2D::angle(npt - pt);
        //qDebug() << point << r1 << r2;
        qreal start = qMin(r1, r2);
        qreal end = qMax(r1, r2);
        qreal length = end - start;
        if (end - start > 180.0) {
            length -= 360.0;
        }
        path.arcMoveTo(bound, start);
        path.arcTo(bound, start, length);
        path.addText(txt, QFont(), QString("%1°").arg(qRound(angle)));
    }
}

