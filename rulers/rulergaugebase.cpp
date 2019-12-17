#include "rulergaugebase.h"
#include "rulergaugecontrol.h"

#include <QEvent>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QtMath>
#include <QDebug>

#include <core/control.h>

RulerGaugeBase::RulerGaugeBase(QGraphicsItem *parent): RulerGaugeBase(500,500,parent)
{
}

RulerGaugeBase::RulerGaugeBase(int width,int height,QGraphicsItem *parent):QGraphicsItem(parent),width_(width),height_(height)
{
    adjustSizeItem = new QGraphicsPixmapItem(this);
    adjustSizeItem->setPixmap(QPixmap(":/icon/icon/add.svg"));
    adjustSizeItem->setAcceptedMouseButtons(Qt::LeftButton);
    rotationItem= new QGraphicsPixmapItem(this);
    rotationItem->setScale(2.0);
    rotationItem->setPixmap(QPixmap(":/icon/icon/remove.svg"));
    rotationItem->setAcceptedMouseButtons(Qt::LeftButton);
    adjustSizeItem->setScale(2.0);
}

RulerGaugeBase::~RulerGaugeBase()
{

}

QRectF RulerGaugeBase::boundingRect() const
{
    return QRectF(0,0,width_,height_);
}

QPainterPath RulerGaugeBase::shape() const
{
    return shape_;
}

bool RulerGaugeBase::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if(watched != rotationItem && watched!=adjustSizeItem){
        return false;
    }
    QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:{
        isPressed = true;
        movePoint = mouseEvent->scenePos();
        break;
    }
    case QEvent::GraphicsSceneMouseMove:
    { QPointF mousePos = mouseEvent->scenePos();
        RulerGaugeControl *control = qobject_cast<RulerGaugeControl*>(RulerGaugeControl::fromItem(this));
        if(watched == rotationItem){
            control->rotate(mapToParent({0,0}), movePoint, mousePos);
        }else if(watched == adjustSizeItem){
            QPointF move=adjustSize(movePoint,mousePos);
            control->move(move);
            control->sizeChanged();
            adjustControlButtonPos();

        }
        movePoint = mousePos;

    }
        break;
    case QEvent::GraphicsSceneMouseRelease:
        isPressed = false;

        break;
    }

    return true;
}

QVariant RulerGaugeBase::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemVisibleHasChanged:
        rotationItem->installSceneEventFilter(this);
        adjustSizeItem->installSceneEventFilter(this);
        break;
    }
    return value;
}

void RulerGaugeBase::adjustControlButtonPos()
{
    QVector<QPointF> points = getControlButtonPos();
    adjustSizeItem->setPos(points[1]);
    rotationItem->setPos(points[2]);
}

QPointF RulerGaugeBase::adjustSize(QPointF from, QPointF to)
{
    return QPointF(0,0);
}

int RulerGaugeBase::getWidth() const
{
    return this->width_;
}

int RulerGaugeBase::getHeight() const
{
    return this->height_;
}

void RulerGaugeBase::paintCalibration(QPainter *painter, int length)
{
    int currentDistance = PxPerMilliMeter;
    while (currentDistance < length) {
        int calibrationHeight = 10;
        if((currentDistance/PxPerMilliMeter)%10 == 0){
            calibrationHeight = 30;
        }else if((currentDistance/PxPerMilliMeter)%5 == 0){
            calibrationHeight = 20;
        }

        painter->drawLine(currentDistance,0,currentDistance,calibrationHeight);

        currentDistance += PxPerMilliMeter;
    }
}
