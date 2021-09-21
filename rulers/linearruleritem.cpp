#include "linearruleritem.h"

#include <QPainter>
#include <QPen>

LinearRulerItem::LinearRulerItem(QGraphicsItem * parent)
    : LinearRulerItem(500, 100, parent)
{
}

LinearRulerItem::LinearRulerItem(qreal width, qreal height, QGraphicsItem *parent)
    : RulerItem(width, height, parent)
{
    updateShape();
    adjustControlButtonPos();
}

LinearRulerItem::~LinearRulerItem()
{ 
}

QVector<QPointF> LinearRulerItem::getControlButtonPos()
{
    return QVector<QPointF>{
        {40, 70},
        {width_ - 100, 70},
        {width_ - 40, 70},
    };
}

void LinearRulerItem::updateShape()
{
    shape_ = QPainterPath();
    shape_.addRoundedRect(boundingRect(), Unit, Unit);
    shape2_ = QPainterPath();
    shape2_.addRect(boundingRect().adjusted(0, Unit * 3, 0, 0));
    RulerItem::updateShape();
}

void LinearRulerItem::onDraw(QPainter *painter)
{
    QPointF corner{Unit / 2 * 5, Unit / 2};
    drawTickMarks(painter, corner, {width_ - corner.x(), corner.y()}, 0);
}
