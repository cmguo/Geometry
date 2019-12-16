#include "rulergaugebase.h"

#include <QEvent>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QtMath>
#include <QDebug>

RulerGaugeBase::RulerGaugeBase(QGraphicsItem *parent): RulerGaugeBase(500,500,parent)
{
}

RulerGaugeBase::RulerGaugeBase(int width,int height,QGraphicsItem *parent):QGraphicsItem(parent),width_(width),height_(height)
{
    adjustSizeItem = new QGraphicsPixmapItem(this);
    adjustSizeItem->setPixmap(QPixmap(":/icon/icon/add.svg"));
    adjustSizeItem->setAcceptedMouseButtons(Qt::LeftButton);
    rotationItem= new QGraphicsPixmapItem(this);
    rotationItem->setPixmap(QPixmap(":/icon/icon/remove.svg"));
    rotationItem->setAcceptedMouseButtons(Qt::LeftButton);
    adjustSizeItem->setPos(QPoint(400,100));
    rotationItem->setPos(QPoint(300,200));
}

RulerGaugeBase::~RulerGaugeBase()
{

}

QRectF RulerGaugeBase::boundingRect() const
{
    return QRectF(0,0,width_,height_);
}

bool RulerGaugeBase::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:{
        isPressed = true;
        break;
    }
    case QEvent::GraphicsSceneMouseMove:
    {
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

void RulerGaugeBase::rotation()
{

}

void RulerGaugeBase::adjustSize()
{

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
