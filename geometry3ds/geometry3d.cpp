#include "geometry3d.h"
#include "geometryhelper.h"

#include <core/resource.h>

#include <QtMath>

/*
 *              Z ^      _/ Y
 *                |    _/
 *                |  _/
 *                | /
 *                o---------------> X
 */
QMatrix4x4 Geometry3D::PO(
        1, float(CO), 0, 0,
        0, -float(CO), -1, 0,
        0, 0, 0, 0,
        0, 0, 0, 1);

/*
 *              Z ^
 *                |
 *                |
 *                |
 *                o
 *              /   \
 *           /         \
 *        /               \
 *   X /                     \ Y
 */
QMatrix4x4 Geometry3D::PI(
        -float(CIX), float(CIX), 0, 0,
        float(CIY), float(CIY), -float(CIY) * 2, 0,
        0, 0, 0, 0,
        0, 0, 0, 1);

Geometry3D::Geometry3D(Resource * res)
    : Geometry(res)
{
}

Geometry3D::Geometry3D(Geometry3D const & o)
    : Geometry(o)
    , origin_(o.origin_)
    , size_(o.size_)
{
}

bool Geometry3D::empty() const
{
    return Geometry::empty() && res_->url().path().lastIndexOf('/') == 0
            && res_->url().query().length() == 0;
}

qreal Geometry3D::length(QVector3D const & vec)
{
    (void) vec;
    return 0;//sqrt(QPointF::dotProduct(vec, vec));
}

void Geometry3D::adjustToLength(const QVector3D &, QVector3D &, qreal)
{
}

void Geometry3D::addLine(QPainterPath &ph, const QPointF &start, const QPointF &end)
{
    ph.moveTo(start);
    ph.lineTo(end);
}

void Geometry3D::addArc(QPainterPath &ph, const QRectF &rect, const QPointF &start, qreal startAngle, qreal arcLength)
{
    ph.moveTo(start);
    ph.arcTo(rect, startAngle, arcLength);
}

void Geometry3D::addDotLine(QPainterPath &ph, const QPointF &start, const QPointF &end, qreal width)
{
    QPointF u = end - start;
    qreal l = GeometryHelper::length(u);
    u = u * width / l;
    QPointF ps = start;
    while (l >= width * 4) {
        ph.moveTo(ps);
        ph.lineTo(ps + u);
        ps = ps + u * 4;
        l -= width * 4;
    }
    if (l > width)
        l = width;
    if (l > 0) {
        ph.moveTo(ps);
        ph.lineTo(ps + u * l / width);
    }
}

static qreal dashArc(qreal a, qreal b, qreal t, qreal w)
{
    qreal d = b - a;
    if (qFuzzyIsNull(d))
        return w / a;
    if (t > M_PI_2 * 3)
        return dashArc(b, a, M_PI * 2 - t, w);
    else if (t > M_PI)
        return dashArc(a, b, t - M_PI, w);
    else if (t > M_PI_2)
        return dashArc(b, a, M_PI - t, w);
    d *= M_2_PI;
    qreal s = a + d * t;
    qreal l = s * s + w * d * 2;
    if (l < 0) {
        //qDebug() << t * 180 / M_PI;
        return w / (a + d * 0.5);
    }
    return qMin(w / (a + d * 0.5), (sqrt(l) - s) / d);
}

void Geometry3D::addDotArc(QPainterPath &ph, const QRectF &rect, QPointF const & start, qreal startAngle, qreal arcLength, qreal width)
{
    qreal a = rect.width() * 0.5;
    qreal b = rect.height() * 0.5;
    QPointF c = rect.center();
    if (arcLength < 0) width = -width;
    ph.moveTo(start); // TODO:
    while (true) {
        qreal r = startAngle * M_PI / 180;
        QPointF p = {a * cos(r), -b * sin(r)};
        //qreal d = sqrt(QPointF::dotProduct(p, p));
        //r = atan(width / d) * 180 / M_PI;
        r = dashArc(a, b, r, width) * 180 / M_PI;
        //qDebug() << startAngle << r;
        if (qAbs(r) > qAbs(arcLength))
            r = arcLength;
        ph.moveTo(c + p);
        ph.arcTo(rect, startAngle, r);
        if (qAbs(r) >= qAbs(arcLength))
            break;
        startAngle += r;
        arcLength -= r;
        r = startAngle * M_PI / 180;
        //p = {a * cos(r), b * sin(r)};
        //d = sqrt(QPointF::dotProduct(p, p));
        //r = atan(width / d * 3) * 180 / M_PI;
        r = dashArc(a, b, r, width * 3) * 180 / M_PI;
        //qDebug() << startAngle << r;
        if (qAbs(r) >= qAbs(arcLength))
            break;
        startAngle += r;
        arcLength -= r;
    }
}

QPainterPath Geometry3D::combine(QPainterPath const & ph1, QPainterPath const & ph2)
{
    QPainterPathStroker ps;
    ps.setCapStyle(Qt::RoundCap);
    ps.setWidth(width_);
    QPainterPath ph = ps.createStroke(ph1);
    QVector<qreal> solid;
    ps.setCapStyle(Qt::FlatCap);
    ps.setDashPattern(QVector<qreal>{2.0, 2.0});
    ph |= ps.createStroke(ph2);
    return ph;
}


Geometry3DFactory::Geometry3DFactory()
{
}

ResourceView * Geometry3DFactory::create(Resource *res)
{
    QString type = res->url().path().mid(1);
    int n = type.indexOf('/');
    if (n > 0)
        type = type.left(n);
    return ResourceFactory::create(res, type.toUtf8());
}

QUrl Geometry3DFactory::newUrl(const QByteArray &type) const
{
    return QUrl("geometry3d:///" + type);
}
