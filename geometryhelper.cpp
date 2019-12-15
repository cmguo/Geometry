#include "geometryhelper.h"

#include <QtMath>

qreal GeometryHelper::angle(QPointF const & vec) // axis angle
{
    if (qFuzzyIsNull(vec.x()))
        return vec.y() > 0 ? 270.0 : 90.0;
    //if (qFuzzyIsNull(vec.y()))
    //    return vec.x() > 0 ? 0.0 : 180.0;
    qreal r = atan(-vec.y() / vec.x());
    if (vec.x() < 0)
        r += M_PI;
    if (r < 0)
        r += M_PI * 2.0;
    //qDebug() << vec << r * 180.0 / M_PI;
    return r * 180.0 / M_PI;
}

qreal GeometryHelper::angle(QPointF const & p1, QPointF const & p2, QPointF const & p3)
{
    qreal dot = dotProduct(p1 - p2, p3 - p2);
    qreal mod1 = dotProduct(p1 - p2, p1 - p2);
    qreal mod2 = dotProduct(p3 - p2, p3 - p2);
    return acos(dot / sqrt(mod1 * mod2)) * 180 / M_PI;
}

qreal GeometryHelper::length(QPointF const & vec)
{
    return sqrt(dotProduct(vec, vec));
}

qreal GeometryHelper::length2(QPointF const & vec)
{
    return dotProduct(vec, vec);
}

void GeometryHelper::rotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() - pt.y() * angle.y(),
                 pt.x() * angle.y() + pt.y() * angle.x());
}

void GeometryHelper::reverseRotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() + pt.y() * angle.y(),
                 -pt.x() * angle.y() + pt.y() * angle.x());
}

qreal GeometryHelper::determinant(QPointF const & p1, QPointF const & p2)
{
    return p1.x() * p2.y() - p2.x() * p1.y();
}

qreal GeometryHelper::dotProduct(const QPointF &p1, const QPointF &p2)
{
    return QPointF::dotProduct(p1, p2);
}

qreal GeometryHelper::dist2PointToSegment(QPointF const & p1, QPointF const & p2,
                                QPointF const & p, QPointF & rp)
{
    QPointF d = p2 - p1;
    qreal dot1 = dotProduct(d, p - p1);
    if (dot1 <= 0)
        return length2(d);
    qreal dot2 = length2(p1 - p2);
    if (dot2 <= dot1)
        return length2(p - p2);
    qreal r = dot1 / dot2;
    rp = p1 + d * r;
    return length2(p - rp);
}

QPointF GeometryHelper::nearestPointAtVerticalBisector(QPointF const & pt1, QPointF const & pt2,
                                   QPointF const & p)
{
    QPointF c = (pt1 + pt2) / 2;
    QPointF d = pt1 - pt2;
    d = QPointF(-d.y(), d.x());
    qreal dot1 = dotProduct(d, p - c);
    qreal dot2 = length2(d);
    qreal r = dot1 / dot2;
    return c + d * r;
}

void GeometryHelper::adjustToLength(QPointF const & start, QPointF & end, qreal length)
{
    QPointF d = end - start;
    d *= length / sqrt(length2(d));
    end = start + d;
}

QPointF GeometryHelper::crossPoint(QPointF const & p1, QPointF const & p2,
                          QPointF const & q1, QPointF const & q2)
{
    QPointF a(p1.y() - p2.y(), q1.y() - q2.y()); // a1, a2
    QPointF b(p2.x() - p1.x(), q2.x() - q1.x()); // b1, b2
    QPointF c(determinant(p1, p2), determinant(q1, q2));
    qreal d = determinant(a, b);
    return QPointF(determinant(b, c), determinant(c, a)) / d;
}

void GeometryHelper::attachToLine(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    QPointF d = p2 - p1;
    qreal dot1 = dotProduct(d, p - p1);
    qreal dot2 = length2(d);
    qreal r = dot1 / dot2;
    QPointF rp = p1 + d * r;
    if (length2(rp - p) < HIT_DIFF_DIFF)
        p = rp;
}

int GeometryHelper::attachToPoints(QVector<QPointF> const & pts, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    int result = -1;
    for (int i = 0; i < pts.size(); ++i) {
        QPointF const & pt = pts[i];
        qreal r = length2(pt - p);
        if (r < min) {
            result = i;
            min = r;
        }
    }
    if (result >= 0)
        p = pts[result];
    return result;
}

static const qreal SQRT3W = 173.20508075688772935274463415059;

static QVector<QPointF> ATTACH_DIRS = {
    QPointF(100, 100), QPointF(100, -100), // 45° 135°
    QPointF(200, 0), QPointF(0, 200), // 0° 90°
    QPointF(100, SQRT3W), QPointF(100, -SQRT3W), // 60° 120°
    QPointF(SQRT3W, 100), QPointF(SQRT3W, -100) // 30° 150°
};

void GeometryHelper::attachToLines(QPointF const & p1, QPointF & p)
{
    attachToLines(p1, ATTACH_DIRS, p);
}

void GeometryHelper::attachToLines(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    attachToLines(p1, p2, ATTACH_DIRS, p);
}

int GeometryHelper::attachToLines(QPointF const & p1, QVector<QPointF> const & dirs, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    QPointF result;
    int index = -1;
    QPointF d1 = p - p1;
    for (int i = 0; i < dirs.size(); ++i) {
        QPointF d = dirs[i];
        qreal dot1 = dotProduct(d, d1);
        qreal dot2 = length2(d);
        qreal r = dot1 / dot2;
        QPointF rp = p1 + d * r;
        r = length2(rp - p);
        if (r < min) {
            result = rp;
            min = r;
            index = i;
        }
    }
    if (min < HIT_DIFF_DIFF)
        p = result;
    return index;
}

int GeometryHelper::attachToLines(QPointF const & p1, QPointF const & p2, QVector<QPointF> const & dirs, QPointF & p)
{
    qreal l = length(p2 - p1);
    QPointF tr = (p2 - p1) / l;
    QPointF pt = p - p1;
    reverseRotate(pt, tr);
    pt += p1;
    //QPointF t1 = pt;
    int index = attachToLines(p1, dirs, pt);
    if (index < 0)
        return index;
    //qDebug() << "attachToLines" << p1 << p2;
    //qDebug() << "attachToLines" << t1 << pt;
    pt -= p1;
    rotate(pt, tr);
    pt += p1;
    //qDebug() << "attachToLines" << p << pt;
    p = pt;
    return index;
}

int GeometryHelper::attachLineToPointsAndLines(const QVector<QPointF> &pts, QVector<int> lines, QPointF &lp1, QPointF &lp2)
{
    (void) lines;
    qreal min1 = HIT_DIFF_DIFF,  min2 = HIT_DIFF_DIFF;
    int result1 = -1,  result2 = -1;
    for (int i = 0; i < pts.size(); ++i) {
        QPointF const & pt = pts[i];
        qreal r = length2(pt - lp1);
        if (r < min1) {
            result1 = i;
            min1 = r;
        } else if (r < min2) {
            result2 = i;
            min2 = r;
        }
    }
    return result1;
}

