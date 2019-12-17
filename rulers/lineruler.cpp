#include "lineruler.h"

#include <QPainter>
#include <QPen>

Lineruler::Lineruler(QGraphicsItem *parent):RulerGaugeBase(parent)
{
    
}

Lineruler::Lineruler(int width, int height, QGraphicsItem *parent):RulerGaugeBase(width,height,parent)
{
    updateShape();
    adjustControlButtonPos();
}

Lineruler::~Lineruler()
{
    
}

void Lineruler::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen p = QPen(Qt::black,2);
    p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    painter->setBrush(Qt::white);
    painter->drawPath(shape_);

    painter->setPen(p);
    paintCalibration(painter,boundingRect().width());
    painter->setBrush(Qt::red);
    painter->drawRect(boundingRect().adjusted(0,40,0,0));
}

QPointF Lineruler::adjustSize(QPointF from,QPointF to)
{
    int diff = to.x()-from.x();
    width_ = width_+diff;
    if(width_<100){
        width_ = 100;
    }
    updateShape();
    return QPointF(0,0);
}

QVector<QPointF> Lineruler::getControlButtonPos()
{
    QVector<QPointF> points;
    points.insert(0,QPointF(0,0));
    points.insert(1,QPointF(100,100));
    points.insert(2,QPointF(200,200));
    return points;
}

void Lineruler::updateShape()
{
    shape_ = QPainterPath();
    shape_.addRect(boundingRect());
}
