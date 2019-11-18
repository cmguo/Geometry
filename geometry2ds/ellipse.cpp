#include "ellipse.h"

Ellipse::Ellipse(Resource * res)
    : Geometry2D(res)
{
}

Ellipse::Ellipse(QPointF const & pt)
    : Geometry2D(pt)
{
}

Ellipse::Ellipse(Ellipse const & o)
    : Geometry2D(o)
{
}

QPainterPath Ellipse::path()
{
    QPainterPath ph;
    if (points_.size() > 1)
    {
        QPointF off(points_.back() - points_.front());
        off.setX(qAbs(off.x()) * sqrt(2.0));
        off.setY(qAbs(off.y()) * sqrt(2.0));
        ph.addEllipse(points_.front(), off.x(), off.y());
    }
    return ph;
}

int Ellipse::hit(QPointF &pt)
{
    QPointF center = (points_.back() + points_.front()) / 2.0;
    QPointF off(points_.back() - center);
    qreal prod = QPointF::dotProduct(off, off);
    QPointF off2(pt - center);
    qreal prod2 = QPointF::dotProduct(off2, off2);
    off = off2 * sqrt(prod / prod2);
    off2 -= off;
    if (QPointF::dotProduct(off2, off2) < 25.0) {
        pt = center + off;
        return 1;
    }
    return -1;
}

