#include "polyhedron.h"
#include "base/geometryhelper.h"

#include <QPainter>

//#define POLYHEDRON_DEBUG

Polyhedron::Polyhedron(Resource * res)
    : Geometry3D(res)
{
}

Polyhedron::Polyhedron(Polyhedron const & o)
    : Geometry3D(o)
    , lines_(o.lines_)
{
}

bool Polyhedron::commitPoint(const QPointF &pt)
{
    if (metaObject() != &Polyhedron::staticMetaObject) {
        return Geometry3D::commitPoint(pt);
    }
    if (pointCount() < 4)
        return false;
    QPointF d = pt - points_.first();
    if (GeometryHelper::length2(d) >= GeometryHelper::HIT_DIFF_DIFF)
        return false;
    points_.pop_back();
    return true;
}

bool Polyhedron::canFinish()
{
    return pointCount() > 2;
}

QPainterPath Polyhedron::visualPath()
{
    QPainterPath ph;
    if (pointCount() < 4)
        return ph;
    QVector<QPointF> points(pointCount());
    QVector<bool> hidden(pointCount());
    collect(points, hidden);
#ifdef POLYHEDRON_DEBUG
    for (int i = 0; i < pointCount(); ++i) {
        ph.addText(points[i] + QPointF(10, -5), QString("%1").arg(i));
    }
#endif
    QPainterPath ph2;
    int last = -1, last2 = -1;
    for (int l : lines_) {
        int s = l & 0xff;
        int e = (l >> 8) & 0xff;
        bool h = hidden[s] || hidden[e] || (l >> 16);
        if (h) {
            if (s != last2)
                ph2.moveTo(points[s]);
            ph2.lineTo(points[e]);
            last2 = e;
        } else {
            if (s != last)
                ph.moveTo(points[s]);
            ph.lineTo(points[e]);
            last = e;
        }
    }
    return combine(ph, ph2);
}

QPainterPath Polyhedron::contour()
{
    QPainterPath ph;
    if (pointCount() < 4)
        return ph;
    ph.setFillRule(Qt::WindingFill);
    QVector<QPointF> points(pointCount());
    QVector<bool> hidden(pointCount());
    collect(points, hidden);
    /*
    int last = -1;
    for (int l :lines_) {
        int s = l & 0xff;
        int e = (l >> 8) & 0xff;
        //if (hidden[s] || hidden[e]) continue;
        if (s != last)
            ph.moveTo(points[s]);
        ph.lineTo(points[e]);
        last = e;
    }
    */
    ph.addPolygon(GeometryHelper::smallestEnclosingPolygon(points));
#ifdef POLYHEDRON_DEBUG
    for (int i = 0; i < pointCount(); ++i) {
        ph.addText(points[i] + QPointF(10, -5), QFont(), QString("%1").arg(i));
    }
#endif
    return ph;
}

int Polyhedron::pointCount()
{
    return points_.size();
}

QVector3D Polyhedron::point(int index)
{
    return QVector3D(points_[index]);
}

bool Polyhedron::setPoint(int index, const QVector3D &pt)
{
    points_[index] = pt.toPointF();
    return true;
}

QVector<QPointF> Polyhedron::movePoints()
{
    QVector<QPointF> points(pointCount());
    QVector<bool> hidden(pointCount());
    collect(points, hidden);
    return points;
}

int Polyhedron::hit(QPointF &pt)
{
    QVector<QPointF> points(pointCount());
    QVector<bool> hidden(pointCount());
    collect(points, hidden);
    return GeometryHelper::attachToPoints(points, pt);
}

bool Polyhedron::moveElememt(int elem, const QPointF &pt)
{
    return Geometry3D::moveElememt(elem, pt);
}

void Polyhedron::clearLines()
{
    lines_.clear();
    dirty_ = true;
}

void Polyhedron::makeLine(int startIndex, int endIndex)
{
    lines_.append(startIndex | endIndex << 8);
}

void Polyhedron::collect(QVector<QPointF> &points, QVector<bool> &hidden)
{
    QVector<QVector3D> point3ds(pointCount() + lines_.count());
    QVector<QVector3D*> orders(pointCount() + lines_.count());
    int i = 0;
    for (; i < pointCount(); ++i) {
        point3ds[i] = point(i);
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        points[i] = PO.map(point3ds[i]).toPointF();
#else
        points[i] = PI.map(point3ds[i]).toPointF();
#endif
        orders[i] = &point3ds[i];
        hidden[i] = true;
    }
    for (int j = 0 ; j < lines_.size(); ++j, ++i) {
        lines_[j] &= 0xffff; // clear old
        int l = lines_[j];
        int r = l >> 8;
        l &= 0xff;
        point3ds[i] = (point3ds[l] + point3ds[r]) / 2; // line center
        orders[i] = &point3ds[i];
    }
    i -= lines_.count(); // pointCount()
    // sort by y
    std::sort(orders.begin(), orders.end(), [](QVector3D*l, QVector3D*r) {
        return l->y() < r->y();
    });
    QPolygonF bound;
    QVector<QPointF> pts;
    for (QVector3D * p : orders) {
        QPointF pt(qreal(p->x()) + qreal(p->y()) * CO, qreal(p->z()) + qreal(p->y()) * CO);
        int j = int(p - point3ds.data());
        if (bound.containsPoint(pt, Qt::OddEvenFill)) {
            if (j >= i) {
                int l = lines_[j - i];
                int r = l >> 8;
                l &= 0xff;
                if (hidden[l] || hidden[r])
                    lines_[j - i] |= 0x10000;
            }
        } else if (j < i) {
            hidden[j] = false;
            pts.append(pt);
            if (pts.size() > 2)
                bound = GeometryHelper::smallestEnclosingPolygon(pts);
        }
    }
}

/*
 * Static functions
 */

