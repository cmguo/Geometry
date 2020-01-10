#include "geometryitem.h"

#include <core/resourceview.h>
#include <core/control.h>

#include <QPainter>
#include <QMetaMethod>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>

class GeometryEditItem : public QGraphicsPathItem
{
public:
    GeometryEditItem(QGraphicsItem * parent)
        : QGraphicsPathItem(parent)
    {
        setPen(QPen(Qt::black));
        setBrush(QBrush(Qt::white));
    }

    void setEditPoints(QVector<QPointF> const & points)
    {
        shape_ = QPainterPath();
        QPainterPath drawShape;
        QRectF circle(0, 0, 40, 40);
        QRectF box(0, 0, 14, 14);
        for (QPointF const & pt : points) {
            circle.moveCenter(pt);
            shape_.addEllipse(circle);
            box.moveCenter(pt);
            drawShape.addEllipse(box);
        }
        setPath(drawShape);
    }

    virtual QPainterPath shape() const override
    {
        return shape_;
    }

private:
    QPainterPath shape_;
};

GeometryItem::GeometryItem(QGraphicsItem * parent)
    : QGraphicsPathItem(parent)
{
    editItem_ = new GeometryEditItem(this);
    setFiltersChildEvents(true);
}

void GeometryItem::setEditPoints(const QVector<QPointF> &points)
{
    static_cast<GeometryEditItem*>(editItem_)->setEditPoints(points);
}

void GeometryItem::showEditor(bool show)
{
    editItem_->setVisible(show);
}

void GeometryItem::setPen(const QPen &pen)
{
    QGraphicsPathItem::setPen(pen);
    editItem_->setPen(QPen(Qt::black, pen.width()));
    editItem_->setBrush(QBrush(pen.color()));
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

bool GeometryItem::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if (watched != editItem_) {
        event->ignore();
        return false;
    }
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        static_cast<QGraphicsSceneMouseEvent*>(event)
                ->setFlags(static_cast<Qt::MouseEventFlags>(512));
        Control::fromItem(this)->event(event);
        return event->isAccepted();
    default:
        return QGraphicsPathItem::sceneEventFilter(watched, event);
    }
}
