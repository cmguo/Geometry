#include "geometry2d.h"

#include <core/resource.h>

#include <QtMath>
#include <QFont>

Geometry2D::Geometry2D(Resource * res, Flags flags, Flags clearFlags)
    : Geometry(res, flags, clearFlags)
{
}

Geometry2D::Geometry2D(QPointF const & pt)
    : Geometry("geometry2d")
{
    addPoint(pt);
}

Geometry2D::Geometry2D(Geometry2D const & o)
    : Geometry(o)
{
}

bool Geometry2D::empty() const
{
    return Geometry::empty() && res_->url().path().lastIndexOf('/') == 0
            && res_->url().query().length() == 0;
}

/*
 * Static functions
 */

qreal Geometry2D::angle(QPointF const & vec) // axis angle
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

void Geometry2D::rotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() - pt.y() * angle.y(),
                 pt.x() * angle.y() + pt.y() * angle.x());
}

void Geometry2D::reverseRotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() + pt.y() * angle.y(),
                 -pt.x() * angle.y() + pt.y() * angle.x());
}

qreal Geometry2D::crossProduct(QPointF const & p1, QPointF const & p2)
{
    return p1.x() * p2.y() - p2.x() * p1.y();
}

qreal Geometry2D::dist2PointToSegment(QPointF const & p1, QPointF const & p2,
                                QPointF const & p, QPointF & rp)
{
    QPointF d = p2 - p1;
    qreal dot1 = QPointF::dotProduct(d, p - p1);
    if (dot1 <= 0)
        return QPointF::dotProduct(d, d);
    qreal dot2 = QPointF::dotProduct(p1 - p2, p1 - p2);
    if (dot2 <= dot1)
        return QPointF::dotProduct(p - p2, p - p2);
    qreal r = dot1 / dot2;
    rp = p1 + d * r;
    return QPointF::dotProduct(p - rp, p - rp);
}

QPointF Geometry2D::nearestPointAtVerticalBisector(QPointF const & pt1, QPointF const & pt2,
                                   QPointF const & p)
{
    QPointF c = (pt1 + pt2) / 2;
    QPointF d = pt1 - pt2;
    d = QPointF(-d.y(), d.x());
    qreal dot1 = QPointF::dotProduct(d, p - c);
    qreal dot2 = QPointF::dotProduct(d, d);
    qreal r = dot1 / dot2;
    return c + d * r;
}

void Geometry2D::adjustToLength(QPointF const & start, QPointF & end, qreal length)
{
    QPointF d = end - start;
    d *= length / sqrt(QPointF::dotProduct(d, d));
    end = start + d;
}

qreal Geometry2D::angle(QPointF const & p1, QPointF const & p2, QPointF const & p3)
{
    qreal dot = QPointF::dotProduct(p1 - p2, p3 - p2);
    qreal mod1 = QPointF::dotProduct(p1 - p2, p1 - p2);
    qreal mod2 = QPointF::dotProduct(p3 - p2, p3 - p2);
    return acos(dot / sqrt(mod1 * mod2)) * 180 / M_PI;
}

QPointF Geometry2D::crossPoint(QPointF const & p1, QPointF const & p2,
                          QPointF const & q1, QPointF const & q2)
{
    QPointF a(p1.y() - p2.y(), q1.y() - q2.y()); // a1, a2
    QPointF b(p2.x() - p1.x(), q2.x() - q1.x()); // b1, b2
    QPointF c(crossProduct(p1, p2), crossProduct(q1, q2));
    qreal d = crossProduct(a, b);
    return QPointF(crossProduct(b, c), crossProduct(c, a)) / d;
}

void Geometry2D::moveLine(QPointF const & llpt, QPointF & lpt, QPointF const & pt,
                QPointF & npt, QPointF const & nnpt)
{
    QPointF lp = crossPoint(llpt, lpt, pt, pt + npt - lpt);
    QPointF np = crossPoint(npt, nnpt, pt, pt + npt - lpt);
    lpt = lp;
    npt = np;
}

void Geometry2D::attachToLine(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    QPointF d = p2 - p1;
    qreal dot1 = QPointF::dotProduct(d, p - p1);
    qreal dot2 = QPointF::dotProduct(d, d);
    qreal r = dot1 / dot2;
    QPointF rp = p1 + d * r;
    if (length2(rp - p) < HIT_DIFF_DIFF)
        p = rp;
}

static const qreal SQRT3W = 173.20508075688772935274463415059;

static QVector<QPointF> ATTACH_DIRS = {
    QPointF(100, 100), QPointF(100, -100), // 45° 135°
    QPointF(200, 0), QPointF(0, 200), // 0° 90°
    QPointF(100, SQRT3W), QPointF(100, -SQRT3W), // 60° 120°
    QPointF(SQRT3W, 100), QPointF(SQRT3W, -100) // 30° 150°
};

void Geometry2D::attachToLines(QPointF const & p1, QPointF & p)
{
    attachToLines(p1, ATTACH_DIRS, p);
}

void Geometry2D::attachToLines(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    attachToLines(p1, p2, ATTACH_DIRS, p);
}

int Geometry2D::attachToLines(QPointF const & p1, QVector<QPointF> const & dirs, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    QPointF result;
    int index = -1;
    QPointF d1 = p - p1;
    for (int i = 0; i < dirs.size(); ++i) {
        QPointF d = dirs[i];
        qreal dot1 = QPointF::dotProduct(d, d1);
        qreal dot2 = QPointF::dotProduct(d, d);
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

int Geometry2D::attachToLines(QPointF const & p1, QPointF const & p2, QVector<QPointF> const & dirs, QPointF & p)
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

void Geometry2D::addAngleLabeling(QPainterPath &path, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt)
{
    addAngleLabeling(path, lpt, pt, npt, angle(lpt, pt, npt));
}

void Geometry2D::addAngleLabeling(QPainterPath &path, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt, qreal angle)
{
    QPointF lp = lpt;
    QPointF np = npt;
    adjustToLength(pt, lp, 10.0);
    adjustToLength(pt, np, 10.0);
    QPointF rpt = lp + np - pt;
    //qDebug() << pt << angle;
    if (qFuzzyCompare(angle, 90.0)) {
        path.moveTo(lp);
        path.lineTo(rpt);
        path.lineTo(np);
        //path.addText(txt, QFont(), QString("90°"));
    } else if ((angle < 90) ? (qFuzzyCompare(angle, 30.0)
               || qFuzzyCompare(angle, 45.0) || qFuzzyCompare(angle, 60.0))
               : ((angle < 180) ? (qFuzzyCompare(angle, 120.0)
                  || qFuzzyCompare(angle, 135.0) || qFuzzyCompare(angle, 150.0))
                  : (qFuzzyCompare(angle, 180.0)
                     || qFuzzyCompare(angle, 270.0) || qFuzzyCompare(angle, 360.0)))) {
        adjustToLength(pt, rpt, 30.0);
        QPointF txt = rpt + QPointF(-8, 6);
        QRectF bound(-14.14, -14.14, 28.28, 28.28);
        bound.moveCenter(pt);
        qreal a1 = Geometry2D::angle(lpt - pt);
        qreal a2 = Geometry2D::angle(npt - pt);
        //qDebug() << point << r1 << r2;
        qreal start = qMin(a1, a2);
        qreal end = qMax(a1, a2);
        qreal length = end - start;
        if (qFuzzyIsNull(angle - 180)) {
            start = a1;
            txt = lpt - pt;
            adjustToLength(QPointF(0, 0), txt, 30.0);
            txt = pt - QPointF(-txt.y(), txt.x()) + QPointF(-8, 6);
            if (length < 0)
                txt = pt * 2 - txt + QPointF(-8, 6);
        } else if (angle < 180) {
            if (length > 180.0) {
                length -= 360.0;
            }
        } else {
            if (length < 180.0) {
                length -= 360.0;
            }
            txt = pt * 2 - txt + QPointF(-8, 6);
        }
        path.arcMoveTo(bound, start);
        path.arcTo(bound, start, length);
        path.addText(txt, QFont(), QString("%1°").arg(qRound(angle)));
    }
}


Geometry2DFactory::Geometry2DFactory()
{
}

ResourceView * Geometry2DFactory::create(Resource *res)
{
    QString type = res->url().path().mid(1);
    int n = type.indexOf('/');
    if (n > 0)
        type = type.left(n);
    return ResourceFactory::create(res, type);
}

QUrl Geometry2DFactory::newUrl(const QString &type) const
{
    return "geometry2d:///" + type;
}
