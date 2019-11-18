#include "circle.h"

Circle::Circle(Resource * res)
    : Ellipse(res)
{
}

Circle::Circle(QPointF const & pt)
    : Ellipse(pt)
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

int Circle::hit(QPointF &pt)
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
