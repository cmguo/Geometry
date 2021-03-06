#include "geometry2d.h"
#include "base/geometryhelper.h"

#include <core/resource.h>

#include <QtMath>
#include <QFont>

Geometry2D::Geometry2D(Resource * res, Flags flags, Flags clearFlags)
    : Geometry(res, flags, clearFlags)
{
}

Geometry2D::Geometry2D(Geometry2D const & o)
    : Geometry(o)
{
}

Geometry2D::Geometry2D(const QVector<QPointF> &points)
    : Geometry("geometry2d", points)
{
}

bool Geometry2D::empty() const
{
    return Geometry::empty() && res_->url().path().lastIndexOf('/') == -1
            && res_->url().query().length() == 0;
}

/*
 * Static functions
 */

void Geometry2D::moveLine(QPointF const & llpt, QPointF & lpt, QPointF const & pt,
                QPointF & npt, QPointF const & nnpt)
{
    QPointF lp = GeometryHelper::crossPoint(llpt, lpt, pt, pt + npt - lpt);
    QPointF np = GeometryHelper::crossPoint(pt, pt + npt - lpt, npt, nnpt);
    lpt = lp;
    npt = np;
}

void Geometry2D::addAngleLabeling(QPainterPath &path, QPainterPath &textPath, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt)
{
    addAngleLabeling(path, textPath, lpt, pt, npt, GeometryHelper::angle(lpt, pt, npt));
}

void Geometry2D::addAngleLabeling(QPainterPath &path, QPainterPath &textPath, QPointF const & lpt,
                               QPointF const & pt, QPointF const & npt, qreal angle)
{
    QPointF lp = lpt;
    QPointF np = npt;
    GeometryHelper::adjustToLength(pt, lp, GeometryHelper::HIT_DIFF);
    GeometryHelper::adjustToLength(pt, np, GeometryHelper::HIT_DIFF);
    QPointF rpt = lp + np - pt;
    //qDebug() << pt << angle;
    QPolygonF polygon({lpt, pt, npt, lpt});
    if (qFuzzyCompare(angle, 90.0)) {
        if (polygon.containsPoint(rpt, Qt::OddEvenFill)) {
                    path.moveTo(lp);
                    path.lineTo(rpt);
                    path.lineTo(np);
                    //path.addText(txt, QFont(), QString("90°"));
        }
    } else if ((angle < 90) ? (qFuzzyCompare(angle, 30.0)
               || qFuzzyCompare(angle, 45.0) || qFuzzyCompare(angle, 60.0))
               : ((angle < 180) ? (qFuzzyCompare(angle, 120.0)
                  || qFuzzyCompare(angle, 135.0) || qFuzzyCompare(angle, 150.0))
                  : (qFuzzyCompare(angle, 180.0)
                     || qFuzzyCompare(angle, 270.0) || qFuzzyCompare(angle, 360.0)))) {
        qreal txtDis = GeometryHelper::HIT_DIFF * 1.5 / sin(angle * M_PI / 360);
        if (angle > 90) txtDis += GeometryHelper::HIT_DIFF * 1.5;
        GeometryHelper::adjustToLength(pt, rpt, txtDis);
        QString text = QString("%1°").arg(qRound(angle));
        QPointF txtOff = GeometryHelper::textOffset(text, Qt::AlignCenter);
        QPointF txtPos = rpt + txtOff;
        QRectF bound = QRectF(QPointF(), QSizeF(2, 2) * GeometryHelper::HIT_DIFF * 1.5);
        bound.moveCenter(pt);
        qreal a1 = GeometryHelper::angle(lpt - pt);
        qreal a2 = GeometryHelper::angle(npt - pt);
        //qDebug() << point << r1 << r2;
        qreal start = qMin(a1, a2);
        qreal end = qMax(a1, a2);
        qreal length = end - start;
        if (qFuzzyIsNull(angle - 180)) {
            start = a1;
            txtPos = lpt - pt;
            GeometryHelper::adjustToLength(QPointF(0, 0), txtPos, GeometryHelper::HIT_DIFF * 3);
            txtPos = pt - QPointF(-txtPos.y(), txtPos.x()) + txtOff;
            if (length < 0)
                txtPos = pt * 2 - txtPos + txtOff;
        } else if (angle < 180) {
            if (length > 180.0) {
                length -= 360.0;
            }
        } else {
            if (length < 180.0) {
                length -= 360.0;
            }
            txtPos = pt * 2 - txtPos + txtOff;
        }
        path.arcMoveTo(bound, start);
        path.arcTo(bound, start, length);
        if (polygon.containsPoint(txtPos, Qt::OddEvenFill)
                && polygon.containsPoint(rpt - txtOff, Qt::OddEvenFill))
            textPath.addText(txtPos, GeometryHelper::TEXT_FONT, text);
    }
}


Geometry2DFactory::Geometry2DFactory()
{
}

ResourceView * Geometry2DFactory::create(Resource *res)
{
    QString type = res->url().path();
    int n = type.indexOf('/');
    if (n > 0)
        type = type.left(n);
    return ResourceFactory::create(res, type.toUtf8());
}

QUrl Geometry2DFactory::newUrl(const QByteArray &type) const
{
    return QUrl("geometry2d:" + type);
}
