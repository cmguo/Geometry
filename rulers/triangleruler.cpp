#include "triangleruler.h"

#include <QPainter>
#include <QtMath>

TriangleRuler::TriangleRuler(bool isosceles,QGraphicsItem *parent):RulerGaugeBase(parent),isosceles_(isosceles)
{

}

TriangleRuler::TriangleRuler(bool isosceles,int width, int height, QGraphicsItem *parent):RulerGaugeBase(width,height,parent),isosceles_(isosceles)
{
    updateShape();
    adjustControlButtonPos();
}

TriangleRuler::~TriangleRuler()
{

}

void TriangleRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *)
{
    //绘制背景
    QPen p = QPen(Qt::black,2);
    p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    painter->setBrush(Qt::white);
    painter->drawPath(shape_);

    int angle = isosceles_?45:60;
    double radians = angle * M_PI/180;
    painter->setBrush(Qt::red);

    p.setColor(Qt::black);
    painter->setPen(p);
    // 水平直线
    painter->save();
    QTransform transform = painter->transform();
    transform.translate(0,boundingRect().height());
    transform.rotate(180,Qt::XAxis);
    painter->setTransform(transform);
    paintCalibration(painter,boundingRect().width()-50);
    painter->resetTransform();
    painter->restore();

    // 垂直直线
    painter->save();
    transform = painter->transform();
    transform.rotate(90,Qt::ZAxis);
    transform.rotate(180,Qt::XAxis);
    painter->setTransform(transform);
    paintCalibration(painter,boundingRect().height()-50);
    painter->resetTransform();
    painter->restore();

    // 斜线
    painter->save();
    transform = painter->transform();
    transform.translate(10,10);
    transform.rotate(90-angle,Qt::ZAxis);
    painter->setTransform(transform);
    paintCalibration(painter,sqrt(boundingRect().height()*boundingRect().height()+boundingRect().width()*boundingRect().width())-50);
    painter->resetTransform();
    painter->restore();

    double newRadians = (90-angle) * M_PI/180;
    QPointF innerPoints[3] = {
        QPointF(40, 40*(1.0/tan(radians/2))),
        QPointF(40, boundingRect().height()-40),
        QPointF(boundingRect().width()-40*(1.0/tan(newRadians/2)), boundingRect().height()-40),
    };
    painter->drawPolygon(innerPoints,3);
}


QPointF TriangleRuler::adjustSize(QPointF from,QPointF to)
{   int origHeight = height_;
    int origWidth = width_;
    int diff = to.y()-from.y();
    height_ = height_-diff;
    if(height_<300){
        height_ = 300;
    }
    int angle = isosceles_?45:60;
    width_ = height_*tan(angle * M_PI/180);
    updateShape();
    return QPointF(0,origHeight-height_);
}

QVector<QPointF> TriangleRuler::getControlButtonPos()
{
    QVector<QPointF> points;
    points.insert(0,QPointF(0,0));
    points.insert(1,QPointF(width_/2,height_/2));
    points.insert(2,QPointF(width_/4,height_/4));
    return points;
}

void TriangleRuler::updateShape()
{
    shape_ =QPainterPath();
    QVector<QPointF> vector;
    QPointF points[3] = {
        QPointF(0, 0),
        QPointF(0, boundingRect().height()),
        QPointF(boundingRect().width(), boundingRect().height()),
    };
    for(QPointF point: points)
        vector.append(point);
    shape_.addPolygon(vector);
}
