#include "regularpolygon.h"

#include "core/resource.h"

#include <QtMath>

RegularPolygon::RegularPolygon(Resource * res)
    : Polygon(res)
    , nEdges_(3)
    , nSpan_(1)
{
    QString size(res->property(Resource::PROP_SUB_TYPE).toString().mid(14));
    int n = size.indexOf('.');
    if (n > 0) {
        nEdges_ = size.left(n).toInt();
        nSpan_ = size.mid(n + 1).toInt();
    } else {
        nEdges_ = size.toInt();
    }
    qreal radius = M_PI * 2 * nSpan_ / nEdges_;
    vAngle_ = QPointF(cos(radius), sin(radius));
}

RegularPolygon::RegularPolygon(QPointF const & pt)
    : Polygon(pt)
{
}

RegularPolygon::RegularPolygon(RegularPolygon const & o)
    : Polygon(o)
    , nEdges_(o.nEdges_)
{
}

int RegularPolygon::pointCount()
{
    return points_.size() == 2 ? nEdges_ : 1;
}

QPointF RegularPolygon::iterPoint(int index, QPointF &hint)
{
    QPointF center = points_.front();
    QPointF pt = points_[1];
    hint = pt - center;
    if (index == 0) {
        return pt;
    } else if (index == 1) {
        rotate(hint, vAngle_);
        return center + hint;
    } else if (index == nEdges_ - 1) {
        rotate(hint, QPointF(vAngle_.x(), -vAngle_.y()));
        return center + hint;
    } else {
        qreal radius = M_PI * 2 * nSpan_ * index / nEdges_;
        rotate(hint, QPointF(cos(radius), sin(radius)));
        return center + hint;
    }
}

QPointF RegularPolygon::nextPoint(int index, QPointF &hint)
{
    (void) index;
    rotate(hint, vAngle_);
    return points_.front() + hint;
}

QPointF RegularPolygon::prevPoint(int index, QPointF &hint)
{
    (void) index;
    rotate(hint, QPointF(vAngle_.x(), -vAngle_.y()));
    return points_.front() + hint;
}

bool RegularPolygon::setPoint(int index, const QPointF &pt)
{
    qDebug() << "setPoint" << index << pt;
    (void) index;
    points_[1] = pt;
    return true;
}

bool RegularPolygon::move(int elem, const QPointF &pt)
{
    qDebug() << "move" << elem;
    if (elem < nEdges_) {
        points_[1] = pt;
        return true;
    }
    elem -= nEdges_;
    QPointF hint;
    QPointF pt0 = points_[0];
    QPointF pt1 = iterPoint(elem, hint);
    QPointF pt2 = prevPoint(-1, hint);
    QPointF rp;
    qreal d = sqrt(dist2PointToSegment(pt1, pt2, pt0, rp));
    qreal dd = sqrt(dist2PointToSegment(pt1, pt2, pt, rp));
    pt1 = rp - pt0;
    pt2 = pt - pt0;
    if (QPointF::dotProduct(pt1, pt1) < QPointF::dotProduct(pt2, pt2))
        dd += d;
    else
        dd = d - dd;
    points_[1] = pt0 + (points_[1] - pt0) * dd / d;
    return true;
}
