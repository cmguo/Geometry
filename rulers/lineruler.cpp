#include "lineruler.h"

#include <QPainter>
#include <QPen>

Lineruler::Lineruler(QGraphicsItem *parent):RulerGaugeBase(parent)
{
    
}

Lineruler::Lineruler(int width, int height, QGraphicsItem *parent):RulerGaugeBase(width,height,parent)
{
    
}

Lineruler::~Lineruler()
{
    
}

void Lineruler::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen p = QPen(Qt::black,2);
    p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    painter->setBrush(Qt::white);
    painter->drawRect(boundingRect());
    painter->setPen(p);
    paintCalibration(painter,boundingRect().width());
    painter->setBrush(Qt::red);
    painter->drawRect(boundingRect().adjusted(0,40,0,0));
}

void Lineruler::rotation()
{
    
}

void Lineruler::adjustSize()
{
    
}
