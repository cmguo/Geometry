#include "frustumcone.h"
#include "base/geometryhelper.h"

#include <QPainter>
#include <QtMath>

//#define CIRCLE_OBLIQUE_PROJECTION

FrustumCone::FrustumCone(Resource * res)
    : Geometry3D(res)
{
}

FrustumCone::FrustumCone(FrustumCone const & o)
    : Geometry3D(o)
{
}

qreal FrustumCone::r2(qreal r)
{
    if (points_.size() == 3) {
        return qAbs(points_[2].x() - points_[0].x());
    }
    return r / 2;
}

//      4-0-5
//     /     \
//    /       \
//   2----3----1

QVector<QPointF> FrustumCone::movePoints()
{
    QVector<QPointF> points(points_.mid(0, 2));
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    points.append(QPointF(pt1.x() * 2 - pt2.x(), pt2.y()));
    points.append(QPointF(pt1.x(), pt2.y()));
    qreal r = pt2.x() - pt1.x();
    qreal r2 = this->r2(r);
    if (!qFuzzyIsNull(r2)) {
        QPointF rx2(r2, 0);
        points.append(pt1 - rx2);
        points.append(pt1 + rx2);
    }
    return points;
}

int FrustumCone::hit(QPointF & pt)
{
    QVector<QPointF> pts = movePoints();
    for (int i = 0; i < pts.size(); ++i) {
        QPointF d = pt - pts[i];
        if (GeometryHelper::length2(d) < GeometryHelper::HIT_DIFF_DIFF) {
            pt = pts[i];
            return i;
        }
    }
    return -1;
}

bool FrustumCone::moveElememt(int elem, QPointF const & pt)
{
    QPointF p(pt);
    if (elem == 0)
        p.setX(points_[0].x());
    else if (elem == 2)
        elem = 1;
    else if (elem == 3)
        p.setX(points_[elem = 1].x());
    if (elem < 2)
        return Geometry3D::moveElememt(elem, p);
    if (elem == 4 || elem == 5) {
        points_[0].setY(p.y());
        if (points_.size() == 2)
            points_.append(p);
        else
            points_[2] = p;
        return true;
    }
    return false;
}

#ifndef CIRCLE_OBLIQUE_PROJECTION

QPainterPath FrustumCone::visualPath()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPainterPath ph2;
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = qAbs(pt2.x() - pt1.x());
    qreal r2 = this->r2(r);
    QPointF center(pt1.x(), pt2.y());
    // draw circle, two halfs, one solid line, one dash line
    QRectF rect(-r, -r * CIXY, r * 2, r * 2 * CIXY);
    rect.moveCenter(center);
    {
        addArc(ph, rect, 180.0, 180.0);
        if (pt2.y() > pt1.y())
            addArc(ph2, rect, 0.0, 180.0);
        else
            addArc(ph, rect, 0.0, 180.0);
    }
    // top cirle
    if (!qFuzzyIsNull(r2)) {
        QRectF rect2(-r2, -r2 * CIXY, r2 * 2, r2 * 2 * CIXY);
        rect2.moveCenter(pt1);
        addArc(ph, rect2, 180.0, 180.0);
        if (pt2.y() < pt1.y())
            addArc(ph2, rect2, 0.0, 180.0);
        else
            addArc(ph, rect2, 0.0, 180.0);
    }
    QPointF px1 = pt1;
    QPointF px2 = pt1;
    QPointF py1 = pt1;
    QPointF py2 = pt1;
    if (!qFuzzyIsNull(r2)) {
        QPointF RX2(r2, 0);
        QPointF RY2(0, r2 * CIXY);
        px1 += RX2;
        px2 -= RX2;
        py1 += RY2;
        py2 -= RY2;
    }
    // solid lines
    QPointF RX(r, 0);
    //QPointF RY(0, r * CIXY);
    addLine(ph, px1, center + RX);
    addLine(ph, px2, center - RX);
    if (!qFuzzyIsNull(r2)) {
        if (pt2.y() > pt1.y())
            addLine(ph, px1, px2);
        //addLine(ph, py1, py2);
    }
    //addLine(ph, py1, center + RY);
    // dash lines
    addLine(ph2, pt1, center);
    if (pt2.y() > pt1.y())
        addLine(ph2, center - RX, center + RX);
    //addLine(ph2, center - RY, center + RY);
    if (!qFuzzyIsNull(r2)) {
        if (pt2.y() < pt1.y())
            addLine(ph2, px1, px2);
        //addLine(ph2, py1, py2);
    }
    return combine(ph, ph2);
}

QPainterPath FrustumCone::contour()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    ph.setFillRule(Qt::WindingFill);
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = qAbs(pt2.x() - pt1.x());
    qreal r2 = this->r2(r);
    QPointF center(pt1.x(), pt2.y());
    QPointF RX(r, 0);
    ph.moveTo(center + RX);
    if (qFuzzyIsNull(r2)) {
        ph.lineTo(pt1);
        ph.lineTo(center - RX);
        ph.lineTo(center + RX);
    } else {
        QPointF RX2(r2, 0);
        ph.lineTo(pt1 + RX2);
        ph.lineTo(pt1 - RX2);
        ph.lineTo(center - RX);
        ph.lineTo(center + RX);
        QRectF rect2(-r2, -r2 * CIXY, r2 * 2, r2 * 2 * CIXY);
        rect2.moveCenter(pt1);
        ph.addEllipse(rect2);
    }
    QRectF rect(-r, -r * CIXY, r * 2, r * 2 * CIXY);
    rect.moveCenter(center);
    ph.addEllipse(rect);
    return ph;
}

bool FrustumCone::contains(const QPointF &pt)
{
    if (points_.size() < 2)
        return false;
    QPainterPath ph;
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = qAbs(pt2.x() - pt1.x());
    qreal r2 = this->r2(r);
    QPointF center(pt1.x(), pt2.y());
    QPointF RX(r, 0);
    ph.moveTo(center + RX);
    QPainterPathStroker ps;
    ps.setCapStyle(Qt::SquareCap);
    ps.setWidth(width_);
    if (qFuzzyIsNull(r2)) {
        ph.lineTo(pt1);
        ph.lineTo(center - RX);
        ph.lineTo(center + RX);
        if (ph.contains(pt))
            return true;
        if (ps.createStroke(ph).contains(pt))
            return true;
    } else {
        QPointF RX2(r2, 0);
        ph.lineTo(pt1 + RX2);
        ph.lineTo(pt1 - RX2);
        ph.lineTo(center - RX);
        ph.lineTo(center + RX);
        if (ph.contains(pt))
            return true;
        QRectF rect2(-r2, -r2 * CIXY, r2 * 2, r2 * 2 * CIXY);
        rect2.moveCenter(pt1);
        ph.addEllipse(rect2);
        if (ps.createStroke(ph).contains(pt))
            return true;
    }
    QRectF rect(-r, -r * CIXY, r * 2, r * 2 * CIXY);
    rect.moveCenter(center);
    ph.addEllipse(rect);
    return ph.contains(pt)
            || ps.createStroke(ph).contains(pt);
}

#else

QPainterPath FrustumCone::contour()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = pt2.x() - pt1.x();
    qreal r2 = this->r2(r);
    //qreal h = pt1.y() - pt2.y();
    origin_ = QVector3D(float(pt1.x()), float(0), float(-pt2.y()));
    //size_ = QVector3D(float(r), float(r), float(h));
    QVector3D R(0, float(r), 0);
    QRectF rect(-r, -r, r * 2, r * 2);
    ph.addEllipse(rect);
    QTransform t(1, 0, CO, -CO, pt1.x(), pt2.y());
    ph = t.map(ph);
    if (!qFuzzyIsNull(r2)) {
        QPainterPath ph2;
        QRectF rect2(-r2, -r2, r2 * 2, r2 * 2);
        QTransform t2(1, 0, CO, -CO, pt1.x(), pt1.y());
        ph2.addEllipse(rect2);
        ph.addPath(t2.map(ph2));
    }
    QPointF rotate = {r / sqrt(1 + CO * CO), 0.0}; rotate.setY(rotate.x() * CO);
    QVector3D RO(rotate);
    QPointF pt6 = PO.map(origin_ + RO).toPoint();
    QPointF pt7 = PO.map(origin_ - RO).toPoint();
    if (qFuzzyIsNull(r2)) {
        ph.moveTo(pt6);
        ph.lineTo(pt1);
        ph.lineTo(pt7);
    } else {
        QVector3D origin2 = QVector3D(float(pt1.x()), float(0), float(-pt1.y()));
        QPointF rotate2 = {r2 / sqrt(1 + CO * CO), 0.0}; rotate2.setY(rotate2.x() * CO);
        QVector3D RO2(rotate2);
        QPointF pt4 = PO.map(origin2 + RO2).toPoint();
        QPointF pt5 = PO.map(origin2 - RO2).toPoint();
        ph.moveTo(pt4);
        ph.lineTo(pt6);
        ph.lineTo(pt7);
        ph.lineTo(pt5);
        ph.closeSubpath();
    }
    return ph;
}

QPainterPath FrustumCone::visualPath()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPainterPath ph2;
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    QPointF pt0(pt1.x(), pt2.y());
    QPointF pt3(pt1.x() * 2 - pt2.x(), pt2.y());
    qreal r = pt2.x() - pt1.x();
    qreal r2 = this->r2(r);
    QPointF rotate = {r / sqrt(1 + CO * CO), 0.0}; rotate.setY(rotate.x() * CO);
    // draw circle, two halfs, one solid line, one dash line
    QRectF rect(-r, -r, r * 2, r * 2); // circle
    QTransform t(1, 0, CO, -CO, pt1.x(), pt2.y());
    qreal angle = atan(CO);
    addArc(ph, rect, -angle * 180.0 / M_PI, 180.0);
    addArc(ph2, rect, -angle * 180.0 / M_PI + 180.0, 180.0);
    // top cirle
    if (!qFuzzyIsNull(r2)) {
        QRectF rect2(-r2, -r2, r2 * 2, r2 * 2); // circle
        QTransform t2(1, 0, CO, -CO, pt1.x(), pt1.y());
        QPainterPath ph3;
        ph3.addEllipse(rect2);
        ph |= t2.map(ph3);
    }
    QVector3D RY(0, float(r), 0);
    QPointF pt4 = PO.map(origin_ - RY).toPointF();
    QPointF pt5 = PO.map(origin_ + RY).toPointF();
    QVector3D RO(rotate);
    QPointF pt6 = PO.map(origin_ + RO).toPoint();
    QPointF pt7 = PO.map(origin_ - RO).toPoint();
    QPointF pt12 = pt1;
    QPointF pt13 = pt1;
    QPointF pt14 = pt1;
    QPointF pt15 = pt1;
    QPointF pt16 = pt1;
    QPointF pt17 = pt1;
    if (!qFuzzyIsNull(r2)) {
        QVector3D origin2 = QVector3D(float(pt1.x()), float(0), float(-pt1.y()));
        QVector3D RY2(0, float(r2), 0);
        pt12 = pt1 + QPointF(r2, 0);
        pt13 = pt1 - QPointF(r2, 0);
        pt14 = PO.map(origin2 - RY2).toPointF();
        pt15 = PO.map(origin2 + RY2).toPointF();
        QPointF rotate2 = {r2 / sqrt(1 + CO * CO), 0.0}; rotate2.setY(rotate2.x() * CO);
        QVector3D RO2(rotate2);
        pt16 = PO.map(origin2 + RO2).toPoint();
        pt17 = PO.map(origin2 - RO2).toPoint();
    }
    // solid lines
    addLine(ph, pt16, pt6);
    addLine(ph, pt17, pt7);
    addLine(ph, pt14, pt4);
    if (!qFuzzyIsNull(r2)) {
        addLine(ph, pt12, pt13);
        addLine(ph, pt14, pt15);
    }
    // dash lines
    addLine(ph2, pt2, pt3);
    addLine(ph2, pt4, pt5);
    addLine(ph2, pt15, pt5);
    addLine(ph2, pt1, pt0);
    return combine(ph, ph2);
}

#endif
