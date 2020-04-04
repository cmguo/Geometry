#include "circle.h"
#include "geometryhelper.h"

#include <cmath>

Circle::Circle(Resource * res)
    : Ellipse(res)
{
}

Circle::Circle(Circle const & o)
    : Ellipse(o)
{
}

QPainterPath Circle::path()
{
    QPainterPath ph;
    if (points_.size() > 1)
    {
        QRectF center(-2, -2, 4, 4);
        center.moveCenter(points_.front());
        ph.addEllipse(center);
        QPointF off(points_.back() - points_.front());
        qreal r = sqrt(QPointF::dotProduct(off, off));
        ph.addEllipse(points_.front(), r, r);
    }
    return ph;
}

QVector<QPointF> Circle::movePoints()
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

int Circle::hit(QPointF &pt)
{
    QPointF off(points_.back() - points_.front());
    qreal prod = GeometryHelper::length2(off);
    QPointF off2(pt - points_.front());
    qreal prod2 = GeometryHelper::length2(off2);
    off = off2 * sqrt(prod / prod2);
    off2 -= off;
    if (GeometryHelper::length2(off2) < GeometryHelper::HIT_DIFF_DIFF) {
        pt = points_.front() + off;
        return 1;
    }
    return -1;
}

bool Circle::move(int elem, QPointF const & pt)
{
    if (elem < 4)
        return  Geometry2D::move(1, pt);
    return false;
}
