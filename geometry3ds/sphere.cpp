#include "sphere.h"
#include "geometry2ds/geometry2d.h"

#include <QPen>
#include <QPainter>

Sphere::Sphere(Resource * res)
    : Geometry3D(res)
{
}

Sphere::Sphere(QPointF const & pt)
    : Geometry3D(pt)
{
}

Sphere::Sphere(Sphere const & o)
    : Geometry3D(o)
{
}

QVector<QPointF> Sphere::movePoints()
{
    QVector<QPointF> points;
    QPointF c(points_.front());
    QPointF off(points_.back() - c);
    points.append(points_[1]);
    off = QPointF(-off.y(), off.x());
    points.append(c + off);
    off = QPointF(-off.y(), off.x());
    points.append(c + off);
    off = QPointF(-off.y(), off.x());
    points.append(c + off);
    return points;
}

int Sphere::hit(QPointF &pt)
{
    QPointF off(points_.back() - points_.front());
    qreal prod = QPointF::dotProduct(off, off);
    QPointF off2(pt - points_.front());
    qreal prod2 = QPointF::dotProduct(off2, off2);
    off = off2 * sqrt(prod / prod2);
    off2 -= off;
    if (QPointF::dotProduct(off2, off2) < HIT_DIFF_DIFF) {
        pt = points_.front() + off;
        return 1;
    }
    return -1;
}

bool Sphere::move(int elem, QPointF const & pt)
{
    if (elem < 4)
        return  Geometry3D::move(1, pt);
    return false;
}

QPainterPath Sphere::path()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPointF center(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = Geometry2D::length(center - pt2);
    QRectF circle(0, 0, r * 2, r * 2);
    circle.moveCenter(center);
    ph.addEllipse(circle);
    return ph;
}

void Sphere::draw(QPainter *painter)
{
    if (points_.size() < 2)
        return;
    QPen pen1(color_, width_);
    QPen pen2(color_, width_, Qt::DotLine);
    QPointF center(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = Geometry2D::length(center - pt2);
    QRectF circle(0, 0, r * 2, r * 2);
    circle.moveCenter(center);
    painter->drawEllipse(circle);
    {
        QRectF rect(-r, -r * CIY, r * 2, r * 2 * CIY);
        QPointF rx(r, 0);
        rect.moveCenter(center);
        QPainterPath ph;
        ph.moveTo(center - rx);
        ph.arcTo(rect, 180.0, 180.0);
        painter->setPen(pen1);
        painter->drawPath(ph);
        {
            QPainterPath ph1;
            ph1.moveTo(center + rx);
            ph1.arcTo(rect, 0.0, 180.0);
            painter->setPen(pen2);
            painter->drawPath(ph1);
        }
    }
    {
        QRectF rect(-r * CIY, -r, r * 2 * CIY, r * 2);
        QPointF ry(0, r);
        rect.moveCenter(center);
        QPainterPath ph;
        ph.moveTo(center + ry);
        ph.arcTo(rect, 270.0, 180.0);
        painter->setPen(pen1);
        painter->drawPath(ph);
        {
            QPainterPath ph1;
            ph1.moveTo(center - ry);
            ph1.arcTo(rect, 90.0, 180.0);
            painter->setPen(pen2);
            painter->drawPath(ph1);
        }
    }
}
