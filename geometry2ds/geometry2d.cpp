#include "geometry2d.h"

#include <core/resource.h>

#include <QtMath>

Geometry2D::Geometry2D(Resource * res)
    : Geometry(res)
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

qreal Geometry2D::length(QPointF const & vec)
{
    return sqrt(QPointF::dotProduct(vec, vec));
}

qreal Geometry2D::length2(QPointF const & vec)
{
    return QPointF::dotProduct(vec, vec);
}

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

void Geometry2D::attachToLines(QPointF const & p1, QPointF & p)
{
    attachToLines(p1, {QPointF(100, 100), QPointF(100, -100),
                       QPointF(100, SQRT3W), QPointF(100, -SQRT3W),
                       QPointF(SQRT3W, 100), QPointF(SQRT3W, -100)}, p);
}

void Geometry2D::attachToLines(QPointF const & p1, QVector<QPointF> const & dirs, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    QPointF result;
    for (QPointF const & d : dirs) {
        qreal dot1 = QPointF::dotProduct(d, p - p1);
        qreal dot2 = QPointF::dotProduct(d, d);
        qreal r = dot1 / dot2;
        QPointF rp = p1 + d * r;
        r = length2(rp - p);
        qDebug() << r;
        if (r < min) {
            result = rp;
            min = r;
        }
    }
    if (min < HIT_DIFF_DIFF)
        p = result;
}

void Geometry2D::attachToPoints(QVector<QPointF> const & pts, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    QPointF result;
    for (QPointF const & pt : pts) {
        qreal r = length2(pt - p);
        if (r < min) {
            result = pt;
            min = r;
        }
    }
    if (min < HIT_DIFF_DIFF)
        p = result;
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
