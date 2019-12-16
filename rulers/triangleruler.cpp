#include "triangleruler.h"

#include <QPainter>
#include <QtMath>

TriangleRuler::TriangleRuler(bool isosceles,QGraphicsItem *parent):RulerGaugeBase(parent),isosceles_(isosceles)
{

}

TriangleRuler::TriangleRuler(bool isosceles,int width, int height, QGraphicsItem *parent):RulerGaugeBase(width,height,parent),isosceles_(isosceles)
{

}

TriangleRuler::~TriangleRuler()
{

}

void TriangleRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //绘制背景
    QPen p = QPen(Qt::black,2);
    p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    painter->setBrush(Qt::white);
    QPointF points[3] = {
        QPointF(0, 0),
        QPointF(0, boundingRect().height()),
        QPointF(boundingRect().width(), boundingRect().height()),
    };
    painter->drawPolygon(points,3);

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
    paintCalibration(painter,boundingRect().width());
    painter->resetTransform();
    painter->restore();

    // 垂直直线
    painter->save();
    transform = painter->transform();
    transform.rotate(90,Qt::ZAxis);
    transform.rotate(180,Qt::XAxis);
    painter->setTransform(transform);
    paintCalibration(painter,boundingRect().height());
    painter->resetTransform();
    painter->restore();

    // 斜线
    painter->save();
    transform = painter->transform();
    transform.rotate(90-angle,Qt::ZAxis);
    painter->setTransform(transform);
    paintCalibration(painter,sqrt(boundingRect().height()*boundingRect().height()+boundingRect().width()*boundingRect().width()));
    painter->resetTransform();
    painter->restore();

    QPointF innerPoints[3] = {
        QPointF(40, 40*(1.0f/tan(radians)+1.0f/sin(radians))),
        QPointF(40, boundingRect().height()-40),
        QPointF(boundingRect().width()-40*(1.0f/atan(radians)+1.0f/cos(radians)), boundingRect().height()-40),
    };
    painter->drawPolygon(innerPoints,3);
}

void TriangleRuler::rotation()
{
}

void TriangleRuler::adjustSize()
{

}
