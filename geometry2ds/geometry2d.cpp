#include "geometry2d.h"
#include "geometryhelper.h"

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

void Geometry2D::moveLine(QPointF const & llpt, QPointF & lpt, QPointF const & pt,
                QPointF & npt, QPointF const & nnpt)
{
    QPointF lp = GeometryHelper::crossPoint(llpt, lpt, pt, pt + npt - lpt);
    QPointF np = GeometryHelper::crossPoint(npt, nnpt, pt, pt + npt - lpt);
    lpt = lp;
    npt = np;
}

void Geometry2D::addAngleLabeling(QPainterPath &path, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt)
{
    addAngleLabeling(path, lpt, pt, npt, GeometryHelper::angle(lpt, pt, npt));
}

void Geometry2D::addAngleLabeling(QPainterPath &path, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt, qreal angle)
{
    QPointF lp = lpt;
    QPointF np = npt;
    GeometryHelper::adjustToLength(pt, lp, 10.0);
    GeometryHelper::adjustToLength(pt, np, 10.0);
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
        GeometryHelper::adjustToLength(pt, rpt, 30.0);
        QPointF txt = rpt + QPointF(-8, 6);
        QRectF bound(-14.14, -14.14, 28.28, 28.28);
        bound.moveCenter(pt);
        qreal a1 = GeometryHelper::angle(lpt - pt);
        qreal a2 = GeometryHelper::angle(npt - pt);
        //qDebug() << point << r1 << r2;
        qreal start = qMin(a1, a2);
        qreal end = qMax(a1, a2);
        qreal length = end - start;
        if (qFuzzyIsNull(angle - 180)) {
            start = a1;
            txt = lpt - pt;
            GeometryHelper::adjustToLength(QPointF(0, 0), txt, 30.0);
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
