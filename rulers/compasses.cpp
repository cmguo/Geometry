#include "compasses.h"

Compasses::Compasses(QGraphicsItem *parent):RulerGaugeBase(parent)
{
    adjustControlButtonPos();
}

Compasses::~Compasses()
{

}

void Compasses::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{

}

QPointF Compasses::adjustSize(QPointF ,QPointF )
{
    return QPointF(0,0);
}

QVector<QPointF> Compasses::getControlButtonPos()
{
    QVector<QPointF> points;
    points.insert(0,QPointF(0,0));
    points.insert(1,QPointF(100,100));
    points.insert(2,QPointF(200,200));
    return points;
}

void Compasses::updateShape()
{

}
