#include "geometrycontrol.h"
#include "geometryitem.h"
#include "geometry.h"

#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QPen>

static char const * toolstr =
        "edit()|编辑|:/showboard/icons/icon_delete.png;"
        "setColor(QColor)|颜色|Popup|:/showboard/icons/icon_delete.png;"
        "setLineWidth(qreal)|线宽|Popup|:/showboard/icons/icon_delete.png;";

GeometryControl::GeometryControl(ResourceView * res, Flags flags, Flags clearFlags)
    : Control(res, flags | PositionAtCenter | KeepAspectRatio, clearFlags)
    , editing_(false)
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

void GeometryControl::edit()
{
    GeometryItem * item = static_cast<GeometryItem *>(item_);
    editing_ = true;
    Geometry * geometry = qobject_cast<Geometry *>(res_);
    editPoints_ = geometry->movePoints();
    item->setEditPoints(editPoints_);
    item->showEditor(true);
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

QString GeometryControl::toolsString(QString const & parent) const
{
    if (parent.isEmpty()) {
        return toolstr;
    } else if (parent == "setColor(QColor)") {
        return nullptr;
    } else if (parent == "setLineWidth(qreal)") {
        return nullptr;
    }
    return nullptr;
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

void GeometryControl::select(bool selected)
{
    if (!selected && editing_) {
        //editing_ = false;
        //GeometryItem * item = static_cast<GeometryItem *>(item_);
        //item->showEditor(false);
    }
    Control::select(selected);
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

void GeometryControl::updateGraph(Geometry * geometry)
{
    GeometryItem * item = static_cast<GeometryItem *>(item_);
    QPainterPath ph(geometry->path());
    if (flags_ & SelfTransform)
        ph = res_->transform()->map(ph);
    item->setPath(ph);
    if (editing_) {
        editPoints_ = geometry->movePoints();
        item->setEditPoints(editPoints_);
    }
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
            if (editing_ && (me->flags() & 256)) {
                qreal min = DBL_MAX;
                int mdx = -1;
                for (int i = 0; i < editPoints_.size(); ++i) {
                    QPointF d = pt - editPoints_[i];
                    qreal dd = QPointF::dotProduct(d, d);
                    if (dd < min) {
                        min = dd;
                        mdx = i;
                    }
                }
                hitElem_ = mdx;
                if (hitElem_ >= 0) {
                    qDebug() << "hit" << hitElem_;
                    hitOffset_ = editPoints_[mdx] - me->pos();
                    return true;
                }
            } else if (!editing_) {
                hitElem_ = graph->hit(pt);
                if (hitElem_ >= 0) {
                    hitOffset_ = pt - me->pos();
                    return true;
                }
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
            QPointF pt = me->pos() + hitOffset_;
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
