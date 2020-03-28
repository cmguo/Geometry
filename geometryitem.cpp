#include "geometryitem.h"
#include "geometryhelper.h"

#include <core/resourceview.h>
#include <core/control.h>

#include <QPainter>
#include <QMetaMethod>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QTouchEvent>

class GeometryEditItem : public QGraphicsPathItem
{
public:
    GeometryEditItem(QGraphicsItem * parent)
        : QGraphicsPathItem(parent)
    {
        setPen(QPen(Qt::black, 2.0));
        setBrush(QBrush(Qt::white));
        setAcceptTouchEvents(true);
    }

    void setEditPoints(QVector<QPointF> const & points)
    {
        shape_ = QPainterPath();
        shape_.setFillRule(Qt::FillRule::WindingFill);
        QPainterPath drawShape;
        QRectF circle(0, 0, GeometryHelper::HIT_DIFF * 4, GeometryHelper::HIT_DIFF * 4);
        QRectF box(0, 0, GeometryHelper::HIT_DIFF * 1.4, GeometryHelper::HIT_DIFF * 1.4);
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

    virtual bool sceneEvent(QEvent *event) override
    {
        switch (event->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            break;
        default:
            return QGraphicsPathItem::sceneEvent(event);
        }
        return event->isAccepted();
    }

private:
    QPainterPath shape_;
};

GeometryItem::GeometryItem(QObject * res, QGraphicsItem * parent)
    : QGraphicsPathItem(parent)
    , res_(res)
{
    editItem_ = new GeometryEditItem(this);
    setFiltersChildEvents(true);
    setAcceptTouchEvents(true);
    QMetaObject const * meta = res->metaObject();
    int index = meta->indexOfMethod("contains(QPointF)");
    if (index >= 0)
        methodContains_ = meta->method(index);
    index = meta->indexOfMethod("draw(QPainter*)");
    if (index >= 0)
        methodDraw_ = meta->method(index);
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
    //editItem_->setPen(QPen(Qt::black, pen.width()));
    //editItem_->setBrush(QBrush(pen.color()));
}

bool GeometryItem::contains(const QPointF &point) const
{
    if (methodContains_.isValid()) {
        bool result;
        methodContains_.invoke(res_, Q_RETURN_ARG(bool, result), Q_ARG(QPointF, point));
        return result;
    }
    return QGraphicsPathItem::contains(point);
}

void GeometryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    if (methodDraw_.isValid()) {
        painter->save();
        methodDraw_.invoke(res_, Q_ARG(QPainter*, painter));
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
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
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
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        static_cast<QTouchEvent*>(event)
                ->setTouchPointStates(static_cast<Qt::TouchPointState>(512));
        Control::fromItem(this)->event(event);
        return false;
    default:
        return QGraphicsPathItem::sceneEventFilter(watched, event);
    }
}

