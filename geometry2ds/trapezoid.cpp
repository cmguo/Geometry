#include "trapezoid.h"
#include "geometryhelper.h"

Trapezoid::Trapezoid(Resource * res)
    : Polygon(res)
{
}

Trapezoid::Trapezoid(QPointF const & pt)
    : Polygon(pt)
{
}

Trapezoid::Trapezoid(Trapezoid const & o)
    : Polygon(o)
{
}

static const qreal SQRT3W = 173.20508075688772935274463415059;

static QVector<QPointF> dirs = {
    QPointF(200, 100), QPointF(200, -100),
    QPointF(200, SQRT3W), QPointF(200, -SQRT3W),
    QPointF(SQRT3W, 50), QPointF(SQRT3W, -50)
};

void Trapezoid::movePoint(const QPointF &pt)
{
    Polygon::movePoint(pt);
    GeometryHelper::attachToLines(points_[0], dirs, points_[1]);
}

bool Trapezoid::commitPoint(const QPointF &pt)
{
    Polygon::commitPoint(pt);
    setPoint(1, point(1));
    return true;
}

int Trapezoid::pointCount()
{
    return points_.size() >= 2 ? 4 : 1;
}

bool Trapezoid::setPoint(int index, const QPointF &pt)
{
    if ((index & 1) == 0) {
        points_[index >> 1] = pt;
    } else if (index == 1) {
        if (points_.size() == 3)
            points_[2] = pt;
        else
            points_.append(pt);
    }
    dirty_ = true;
    return true;
}

QPointF Trapezoid::point(int index)
{
    if ((index & 1) == 0)
        return points_[index >> 1];
    if (index == 1 && points_.size() == 3)
        return points_[2];
    QPointF const & st = points_.first();
    QPointF const & pt = points_[1];
    if (index == 1) {
        return QPointF((st.x() + pt.x()) / 2, st.y());
    } else {
        return point4(st, point(1), pt); // implemention defined
    }
}

bool Trapezoid::move(int elem, const QPointF &pt)
{
    if (elem == 1) {
        QPointF p = pt;
        GeometryHelper::attachToLines(points_[1], p);
        Polygon::move(5, p);
        Polygon::setPoint(2, p);
        return true;
    } else if (elem == 2) {
        Polygon::setPoint(1, pt);
        GeometryHelper::attachToLines(points_[2], points_[1]);
        return true;
    } else if (elem < 4) {
        return moveKeepAngle(elem, pt);
    }
    return Polygon::move(elem, pt);
}
