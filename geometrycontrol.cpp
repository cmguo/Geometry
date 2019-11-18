#include "geometrycontrol.h"
#include "geometryitem.h"
#include "geometry.h"

#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QPen>

static char const * toolstr =
        "setColor(QColor)|颜色|:/showboard/icons/icon_delete.png;"
        "setLineWidth(qreal)|线宽|:/showboard/icons/icon_delete.png;";

GeometryControl::GeometryControl(ResourceView * res, Flags flags, Flags clearFlags)
    : Control(res, flags | PositionAtCenter | KeepAspectRatio, clearFlags)
{
}

void GeometryControl::setColor(QColor color)
{
    res_->setProperty("color", color);
    updateSettings();
}

void GeometryControl::setLineWidth(qreal width)
{
    res_->setProperty("width", width);
    updateSettings();
}

void GeometryControl::attached()
{
    updateSettings();
    Geometry * gh = static_cast<Geometry *>(res_);
    if (gh->empty()) {
        loadFinished(true);
    } else {
        QWeakPointer<int> life(this->life());
        gh->load().then([this, gh, life]() {
            if (life.isNull())
                return;
            updateGraph(gh);
            loadFinished(true);
        });
    }
}

QString GeometryControl::toolsString() const
{
    return toolstr;
}

QGraphicsItem * GeometryControl::create(ResourceView * res)
{
    Geometry * gh = qobject_cast<Geometry *>(res);
    QGraphicsPathItem * item = new GeometryItem();
    item->setBrush(QColor(0, 0, 255, 20));
    QWeakPointer<int> life(this->life());
    if (!gh->empty()) {
        gh->load().then([item, gh, life]() {
            if (life.isNull())
                return;
            item->setPath(gh->path());
        });
    }
    return item;
}

void GeometryControl::updateSettings()
{
    QColor color = res_->property("color").value<QColor>();
    qreal width = res_->property("width").toReal();
    setPen(QPen(color, width));
}

Control::SelectMode GeometryControl::selectTest(QPointF const & point)
{
    (void) point;
    return NotSelect;// item_->contains(point) ? Select : PassSelect;
}

void GeometryControl::setPen(const QPen &pen)
{
    QGraphicsPathItem * item = static_cast<QGraphicsPathItem *>(item_);
    item->setPen(pen);
}

void GeometryControl::updateTransform()
{
    if (flags_ & SelfTransform)
        updateGraph(static_cast<Geometry *>(res_));
    else
        Control::updateTransform();
}

void GeometryControl::updateGraph(Geometry * gh)
{
    QGraphicsPathItem * item = static_cast<QGraphicsPathItem *>(item_);
    QPainterPath ph(qobject_cast<Geometry *>(gh)->path());
    if (flags_ & SelfTransform)
        ph = res_->transform()->map(ph);
    item->setPath(ph);
}

QRectF GeometryControl::bounds()
{
    return item_->boundingRect();
}

bool GeometryControl::event(QEvent *event)
{
    Geometry * graph = static_cast<Geometry *>(resource());
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (flags_ & DrawFinished) {
            QPointF pt = me->pos();
            hitElem_ = graph->hit(pt);
            if (hitElem_ >= 0) {
                hitDiff_ = pt - me->pos();
                return true;
            }
        } else {
            graph->addPoint(me->pos());
            updateGraph(graph);
            return true;
        }
        break;
    }
    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (flags_ & DrawFinished) {
            QPointF pt = me->pos() + hitDiff_;
            if (graph->move(hitElem_, pt)) {
                updateGraph(graph);
            }
        } else {
            graph->movePoint(me->pos());
            updateGraph(graph);
        }
        return true;
    }
    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (flags_ & DrawFinished) {
            graph->finish(bounds().center());
            updateTransform();
            updateGraph(graph);
        } else {
            if (graph->commit(me->pos())) {
                graph->finish(bounds().center());
                flags_ |= DrawFinished;
                updateTransform();
            } else if (graph->canFinish()) {
                me->setFlags(static_cast<Qt::MouseEventFlags>(256));
            }
            updateGraph(graph);
        }
        return true;
    }
    case QEvent::User:
        graph->finish(bounds().center());
        flags_ |= DrawFinished;
        updateTransform();
        updateGraph(graph);
        break;
    default:
        break;
    }
    event->ignore();
    return false;
}
