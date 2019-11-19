#include "frustumcone.h"

#include <QPainter>
#include <QtMath>

//#define CIRCLE_OBLIQUE_PROJECTION

FrustumCone::FrustumCone(Resource * res)
    : Geometry3D(res)
{
}

FrustumCone::FrustumCone(QPointF const & pt)
    : Geometry3D(pt)
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

QVector<QPointF> FrustumCone::movePoints()
{
    QVector<QPointF> points(points_);
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    points.append(QPointF(pt1.x() * 2 - pt2.x(), pt2.y()));
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
    int elem = Geometry3D::hit(pt);
    if (elem == 2)
        elem = 3;
    return elem;
}

bool FrustumCone::move(int elem, QPointF const & pt)
{
    QPointF p(pt);
    if (elem == 0)
        p.setX(points_[0].x());
    else if (elem == 2)
        elem = 1;
    if (elem < 2)
        return Geometry3D::move(elem, p);
    if (elem == 3 || elem == 4) {
        p.setY(points_[0].y());
        elem = 3;
        if (points_.size() == 2)
            points_.append(p);
        else
            points_[2] = p;
        return true;
    }
    return false;
}

#ifdef CIRCLE_OBLIQUE_PROJECTION

QPainterPath FrustumCone::path()
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

void FrustumCone::draw(QPainter *painter)
{
    if (points_.size() < 2)
        return;
    QPen pen1(color_, width_);
    QPen pen2(color_, width_, Qt::DashLine);
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
    {
        QPainterPath ph1;
        ph1.moveTo(rotate);
        ph1.arcTo(rect, -angle * 180.0 / M_PI, 180.0);
        painter->setPen(pen1);
        painter->drawPath(t.map(ph1));
    }
    {
        QPainterPath ph2;
        ph2.moveTo(-rotate);
        ph2.arcTo(rect, -angle * 180.0 / M_PI + 180.0, 180.0);
        painter->setPen(pen2);
        painter->drawPath(t.map(ph2));
    }
    // top cirle
    if (!qFuzzyIsNull(r2)) {
        QRectF rect2(-r2, -r2, r2 * 2, r2 * 2); // circle
        QTransform t2(1, 0, CO, -CO, pt1.x(), pt1.y());
        QPainterPath ph3;
        ph3.addEllipse(rect2);
        painter->setPen(pen1);
        painter->drawPath(t2.map(ph3));
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
    painter->setPen(pen1);
    painter->drawLine(pt16, pt6);
    painter->drawLine(pt17, pt7);
    painter->drawLine(pt14, pt4);
    if (!qFuzzyIsNull(r2)) {
        painter->drawLine(pt12, pt13);
        painter->drawLine(pt14, pt15);
    }
    // dash lines
    painter->setPen(pen2);
    painter->drawLine(pt2, pt3);
    painter->drawLine(pt4, pt5);
    painter->drawLine(pt15, pt5);
    painter->drawLine(pt1, pt0);
}

#else

QPainterPath FrustumCone::path()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    ph.setFillRule(Qt::WindingFill);
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = qAbs(pt2.x() - pt1.x());
    //qreal h = (pt2.y() - pt1.y()) / (CIY * 2);
    qreal r2 = this->r2(r);
    //qreal y0 = (pt1.x() * CIY + pt1.y() * CIX) / (CIX * CIY * 2.0) + h;
    //qreal x0 = y0 - pt1.x() / CIX;
    //qreal z0 = 0;
    QPointF center(pt1.x(), pt2.y());
    //origin_ = QVector3D(float(x0), float(y0), float(z0));
    //size_ = QVector3D(float(r), float(r), float(h));
    QPointF RX(r, 0);
    //QPointF RY(0, r * CIXY);
    ph.moveTo(center + RX);
    if (qFuzzyIsNull(r2)) {
        ph.lineTo(pt1);
        ph.lineTo(center - RX);
    } else {
        QPointF R2(r2, 0);
        ph.moveTo(center + RX);
        ph.lineTo(pt1 + R2);
        QRectF rect2(-r2, -r2 * CIXY, r2 * 2, r2 * 2 * CIXY);
        rect2.moveCenter(pt1);
        ph.arcTo(rect2, 0, 180.0);
    }
    ph.lineTo(center - RX);
    QRectF rect(-r, -r * CIXY, r * 2, r * 2 * CIXY);
    rect.moveCenter(center);
    ph.arcTo(rect, 180.0, 180.0);
    return ph;
}

void FrustumCone::draw(QPainter *painter)
{
    if (points_.size() < 2)
        return;
    QPen pen1(color_, width_);
    QPen pen2(color_, width_, Qt::DotLine);
    QPointF pt1(points_[0]);
    QPointF pt2(points_[1]);
    qreal r = qAbs(pt2.x() - pt1.x());
    qreal r2 = this->r2(r);
    QPointF center(pt1.x(), pt2.y());
    QPointF RX(r, 0);
    QPointF RY(0, r * CIXY);
    // draw circle, two halfs, one solid line, one dash line
    QRectF rect(-r, -r * CIXY, r * 2, r * 2 * CIXY);
    rect.moveCenter(center);
    {
        QPainterPath ph2;
        ph2.moveTo(center - RX);
        ph2.arcTo(rect, 180.0, 180.0);
        painter->setPen(pen1);
        painter->drawPath(ph2);
        {
            QPainterPath ph1;
            ph1.moveTo(center + RX);
            ph1.arcTo(rect, 0.0, 180.0);
            if (pt2.y() > pt1.y())
                painter->setPen(pen2);
            painter->drawPath(ph1);
        }
    }
    // top cirle
    QPointF RX2(r2, 0);
    if (!qFuzzyIsNull(r2)) {
        QRectF rect2(-r2, -r2 * CIXY, r2 * 2, r2 * 2 * CIXY);
        rect2.moveCenter(pt1);
        {
            QPainterPath ph2;
            ph2.moveTo(pt1 - RX2);
            ph2.arcTo(rect2, 180.0, 180.0);
            painter->setPen(pen1);
            painter->drawPath(ph2);
        }
        {
            QPainterPath ph1;
            ph1.moveTo(pt1 + RX2);
            ph1.arcTo(rect2, 0.0, 180.0);
            if (pt2.y() < pt1.y())
                painter->setPen(pen2);
            painter->drawPath(ph1);
        }
    }
    QPointF px1 = pt1;
    QPointF px2 = pt1;
    QPointF py1 = pt1;
    QPointF py2 = pt1;
    if (!qFuzzyIsNull(r2)) {
        QPointF RY2(0, r2 * CIXY);
        px1 += RX2;
        px2 -= RX2;
        py1 += RY2;
        py2 -= RY2;
    }
    // solid lines
    painter->setPen(pen1);
    painter->drawLine(px1, center + RX);
    painter->drawLine(px2, center - RX);
    //painter->drawLine(py1, center + RY);
    // dash lines
    painter->setPen(pen2);
    painter->drawLine(pt1, center);
    if (pt2.y() > pt1.y())
        painter->drawLine(center - RX, center + RX);
    //painter->drawLine(center - RY, center + RY);
    if (!qFuzzyIsNull(r2)) {
        if (pt2.y() < pt1.y())
            painter->drawLine(px1, px2);
        //painter->drawLine(py1, py2);
    }
}

#endif
