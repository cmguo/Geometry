#include "polyhedron.h"
#include "geometryhelper.h"

#include <QPainter>

//#define POLYHEDRON_DEBUG

Polyhedron::Polyhedron(Resource * res)
    : Geometry3D(res)
{
}

Polyhedron::Polyhedron(QPointF const & pt)
    : Geometry3D(pt)
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

QPainterPath Polyhedron::path()
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

void Polyhedron::draw(QPainter *painter)
{
    if (pointCount() < 4)
        return;
    QPen pen1(color_, width_);
    QPen pen2(color_, width_, Qt::DotLine);
    QVector<QPointF> points(pointCount());
    QVector<bool> hidden(pointCount());
    collect(points, hidden);
#ifdef POLYHEDRON_DEBUG
    for (int i = 0; i < pointCount(); ++i) {
        painter->drawText(points[i] + QPointF(10, -5), QString("%1").arg(i));
    }
#endif
    for (int l : lines_) {
        int s = l & 0xff;
        int e = (l >> 8) & 0xff;
        bool h = hidden[s] || hidden[e];
        painter->setPen(h ? pen2 : pen1);
        painter->drawLine(points[s], points[e]);
    }
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

bool Polyhedron::move(int elem, const QPointF &pt)
{
    return Geometry3D::move(elem, pt);
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
    QVector<QVector3D> point3ds(pointCount());
    for (int i = 0; i < pointCount(); ++i) {
        point3ds[i] = point(i);
    }
    QVector3D limit = point3ds[0];
//    QVector<QPointF> xy(pointCount());
//    QVector<QPointF> yz(pointCount());
//    QVector<QPointF> zx(pointCount());
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
    points[0] = PO.map(limit).toPointF();
#else
    points[0] = PI.map(limit).toPointF();
#endif
    for (int i = 1; i < pointCount(); ++i) {
        QVector3D pt = point3ds[i];
        limit.setX(qMax(limit.x(), pt.x()));
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        limit.setY(qMin(limit.y(), pt.y()));
#else
        limit.setY(qMax(limit.y(), pt.y()));
#endif
        limit.setZ(qMax(limit.z(), pt.z()));
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        points[i] = PO.map(pt).toPointF();
#else
        points[i] = PI.map(pt).toPointF();
#endif
//        xy[i] = QPointF(pt.x(), pt.y());
//        yz[i] = QPointF(pt.y(), pt.z());
//        zx[i] = QPointF(pt.z(), pt.x());
    }
//    QPolygonF pgXY = GeometryHelper::smallestEnclosingPolygon(xy);
//    QPolygonF pgYZ = GeometryHelper::smallestEnclosingPolygon(yz);
//    QPolygonF pgZX = GeometryHelper::smallestEnclosingPolygon(zx);
    for (int i = 0; i < pointCount(); ++i) {
        QVector3D pt = point3ds[i];
#ifndef POLYHEDRON_ISOMETRIC_PROJECTION
        if (pt.x() < limit.x() && pt.y() > limit.y() && pt.z() < limit.z())
#else
        if (pt.x() < limit.x() && pt.y() < limit.y() && pt.z() < limit.z())
#endif
            hidden[i] = true;
    }
}

/*
 * Static functions
 */

