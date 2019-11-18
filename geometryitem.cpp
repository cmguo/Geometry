#include "geometryitem.h"

#include <core/resourceview.h>
#include <core/control.h>

#include <QPainter>
#include <QMetaMethod>
#include <QEvent>

GeometryItem::GeometryItem(QGraphicsItem * parent)
    : QGraphicsPathItem(parent)
{
}

void GeometryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    ResourceView * res = Control::fromItem(this)->resource();
    int index = res->metaObject()->indexOfMethod("draw(QPainter*)");
    if (index >= 0) {
        painter->save();
        res->metaObject()->method(index).invoke(res, Q_ARG(QPainter*, painter));
        painter->restore();
        return;
    }
    QGraphicsPathItem::paint(painter, option, widget);
}

bool GeometryItem::sceneEvent(QEvent * event)
{
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        Control::fromItem(this)->event(event);
        return event->isAccepted();
    default:
        return QGraphicsPathItem::sceneEvent(event);
    }
}
