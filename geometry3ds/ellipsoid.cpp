#include "ellipsoid.h"
#include "base/geometryhelper.h"

#include <QPen>
#include <QPainter>

#include <cmath>

Ellipsoid::Ellipsoid(Resource * res)
    : Geometry3D(res)
{
}

Ellipsoid::Ellipsoid(Ellipsoid const & o)
    : Geometry3D(o)
    ,radius_(o.radius_)
{
}

void Ellipsoid::sync()
{
    if (dirty_ && points_.size() > 1) {
        QPointF d = points_.back() - points_.first();
        radius_.setX(float(qAbs(d.x())));
        radius_.setY(float(qAbs(d.y())));
        if (!finished())
            radius_.setZ(float(qAbs(d.y())));
        dirty_ = false;
    }
}

QVector<QPointF> Ellipsoid::movePoints()
{
    QVector<QPointF> points;
    QPointF ct(points_.front());
    QPointF dx(qreal(radius_.x()), 0);
    QPointF dy(0, qreal(radius_.y()));
    float a = 6 * pow(radius_.x() * radius_.y() * radius_.z(), 2);
    float b = pow(radius_.z(), 4);
    float c = b - 36 * pow(radius_.x() * radius_.y(), 2);
    QPointF dz(qreal(sqrt((b * pow(radius_.x(), 2) - a) / c)),
               qreal(sqrt((b * pow(radius_.y(), 2) - a) / c)));
    return QVector<QPointF>{ct - dx, ct + dx, ct - dy, ct + dy, ct - dz, ct + dz};
}

int Ellipsoid::hit(QPointF &)
{
    return -1;
}

bool Ellipsoid::moveElememt(int elem, QPointF const & pt)
{
    if (elem < 2) {
        points_.back().setX(pt.x());
        QPointF d = points_.back() - points_.first();
        radius_.setX(float(qAbs(d.x())));
    } else if (elem < 4) {
        points_.back().setY(pt.y());
        QPointF d = points_.back() - points_.first();
        radius_.setY(float(qAbs(d.y())));
    } else if (elem < 6) {
        QPointF d = pt - points_.first();
        qreal c2 = 3 * QPointF::dotProduct(d, d)
                / (1 - pow(d.x() / qreal(radius_.x()), 2) - pow(d.y() / qreal(radius_.y()), 2));
        radius_.setZ(float(sqrt(c2)));
    }
    return true;
}

QPainterPath Ellipsoid::visualPath()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPainterPath ph2;
    QPointF center(points_.front());
    // XY circle
    QRectF circle(0, 0, qreal(radius_.x()) * 2, qreal(radius_.y()) * 2);
    circle.moveCenter(center);
    addArc(ph, circle, 0, 360.0);
    // XZ cicle
    {
        QRectF rect(-qreal(radius_.x()), -qreal(radius_.z()) * CIY,
                    qreal(radius_.x()) * 2, qreal(radius_.z()) * 2 * CIY);
        rect.moveCenter(center);
        addArc(ph, rect, 180.0, 180.0);
        addArc(ph2, rect, 0.0, 180.0);
    }
    // YZ cicle
    {
        QRectF rect(-qreal(radius_.z()) * CIY, -qreal(radius_.y()),
                    qreal(radius_.z()) * CIY * 2, qreal(radius_.y()) * 2);
        rect.moveCenter(center);
        addArc(ph, rect, 270.0, 180.0);
        addArc(ph2, rect, 90.0, 180.0);
    }
    return combine(ph, ph2);
}

QPainterPath Ellipsoid::contour()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPointF center(points_.front());
    QRectF circle(0, 0, qreal(radius_.x()) * 2, qreal(radius_.y()) * 2);
    circle.moveCenter(center);
    ph.addEllipse(circle);
    return ph;
}

