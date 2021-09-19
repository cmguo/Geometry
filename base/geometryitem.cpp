#include "geometryitem.h"
#include "geometryhelper.h"
#include "geometry.h"

#include <core/resourceview.h>
#include <core/control.h>

#include <QPainter>
#include <QMetaMethod>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QTouchEvent>
#include <QCursor>

#ifdef SHOWBOARD_QUICK

#else

class GeometryEditItem : public QGraphicsPathItem
{
public:
    GeometryEditItem(QGraphicsItem * parent)
        : QGraphicsPathItem(parent)
    {
        setPen(QPen(Qt::black, 2.0));
        setBrush(QBrush(Qt::white));
        setAcceptTouchEvents(true);
        setCursor(Qt::CrossCursor);
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

#endif

GeometryItem::GeometryItem(Geometry * geometry, ControlView * parent)
#ifdef SHOWBOARD_QUICK
    : QQuickItem(parent)
#else
    : QGraphicsPathItem(parent)
#endif
    , geometry_(geometry)
{
#ifdef SHOWBOARD_QUICK
#else
    editItem_ = new GeometryEditItem(this);
    setFiltersChildEvents(true);
#endif
    setAcceptTouchEvents(true);
    QMetaObject const * meta = geometry->metaObject();
    int index = meta->indexOfMethod("contains(QPointF)");
    if (index >= 0)
        methodContains_ = meta->method(index);
}

void GeometryItem::setEditPoints(const QVector<QPointF> &points)
{
#ifdef SHOWBOARD_QUICK
    (void) points;
#else
    static_cast<GeometryEditItem*>(editItem_)->setEditPoints(points);
#endif
}

void GeometryItem::showEditor(bool show)
{
    editItem_->setVisible(show);
}

void GeometryItem::setColor(const QColor & color)
{
#ifdef SHOWBOARD_QUICK
    (void) color;
#else
    QGraphicsPathItem::setBrush(color);
#endif
    //editItem_->setPen(QPen(Qt::black, pen.width()));
    //editItem_->setBrush(QBrush(pen.color()));
}

void GeometryItem::setContourPath(const QPainterPath &path)
{
#ifdef SHOWBOARD_QUICK
    (void) path;
#else
    setPath(path);
    update(); // contour may not change
#endif
}

bool GeometryItem::contains(const QPointF &point) const
{
    if (methodContains_.isValid()) {
        bool result;
        methodContains_.invoke(geometry_, Q_RETURN_ARG(bool, result), Q_ARG(QPointF, point));
        return result;
    }
#ifdef SHOWBOARD_QUICK
    return false;
#else
    return QGraphicsPathItem::contains(point);
#endif
}

#ifdef SHOWBOARD_QUICK
#else

void GeometryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                   QWidget *)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(geometry_->color());
    painter->drawPath(geometry_->visualPath());
    painter->restore();
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

#endif
