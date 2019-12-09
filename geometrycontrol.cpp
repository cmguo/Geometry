#include "geometrycontrol.h"
#include "geometryitem.h"
#include "geometry.h"
#include "views/whitecanvas.h"
#include "views/itemselector.h"
#include "geometry2ds/line.h"
#include "geometry2ds/regularpolygon.h"
#include "geometry2ds/sector.h"

#include <core/toolbutton.h>

#include <QFile>
#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QPen>

static char const * toolstr =
        "edit()|调节|HideSelector|:/showboard/icons/edit.svg;"
        "setColor(QColor)|颜色|Popup,OptionsGroup,NeedUpdate|;"
        "-setLineWidth(qreal)|线宽|Popup,OptionsGroup,NeedUpdate|;";

GeometryControl::GeometryControl(ResourceView * res, Flags flags, Flags clearFlags)
    : Control(res, flags | PositionAtCenter | KeepAspectRatio, clearFlags)
    , editing_(false)
{
    if (res_->flags() & ResourceView::DrawFinised)
        flags_ |= DrawFinished;
}

QGraphicsItem * GeometryControl::create(ResourceView * res)
{
    Geometry * geometry = qobject_cast<Geometry *>(res);
    GeometryItem * item = new GeometryItem();
    //item->setBrush(QColor(0, 0, 255, 20));
    item->editItem()->setData(1000 /*ITEM_KEY_CONTROL*/, QVariant::fromValue(this));
    QWeakPointer<int> life(this->life());
    if (!geometry->empty()) {
        geometry->load().then([item, geometry, life]() {
            if (life.isNull())
                return;
            item->setPath(geometry->path());
        });
    }
    return item;
}

void GeometryControl::attached()
{
    updateSettings();
    Geometry * geometry = static_cast<Geometry *>(res_);
    QObject::connect(geometry, &Geometry::changed,
                     this, &GeometryControl::finishGraph);
    if (geometry->empty()) {
        loadFinished(true);
    } else {
        QWeakPointer<int> life(this->life());
        geometry->load().then([this, life]() {
            if (life.isNull())
                return;
            updateGraph();
            loadFinished(true);
        });
    }
}

QString GeometryControl::toolsString(QString const & parent) const
{
    if (parent.isEmpty()) {
        return toolstr;
    }
    return nullptr;
}

static QList<ToolButton *> colorButtons;
static QList<ToolButton *> lineWidthButtons;

static QGraphicsItem* colorIcon(QColor color)
{
    QGraphicsRectItem * item = new QGraphicsRectItem;
    item->setRect({1, 1, 30, 30});
    item->setPen(QPen(QColor(color.red() / 2 + 128, // mix with white
                        color.green() / 2 + 128, color.blue() / 2 + 128), 2.0));
    item->setBrush(color);
    return item;
}

void GeometryControl::getToolButtons(QList<ToolButton *> &buttons, const QList<ToolButton *> &parents)
{
    if (parents.isEmpty()) {
        Control::getToolButtons(buttons, parents);
        static_cast<Geometry *>(res_)->getToolButtons(buttons, nullptr);
        return;
    }
    ToolButton * button = parents.last();
    if (button->name == "setColor(QColor)") {
        if (colorButtons.isEmpty()) {
            for (Qt::GlobalColor c : {
                 Qt::white, Qt::black, Qt::red, Qt::green, Qt::blue, Qt::yellow,
             }) {
                QString name = QVariant::fromValue(c).toString();
                ToolButton::Flags flags = nullptr;
                QGraphicsItem * icon = colorIcon(c);
                ToolButton * btn = new ToolButton({name, "", flags,
                     QVariant::fromValue(icon)});
                colorButtons.append(btn);
            }
        }
        buttons.append(colorButtons);
    } else if (button->name == "setLineWidth(qreal)") {
        if (lineWidthButtons.isEmpty()) {
            QPainterPath ph;
            ph.moveTo(0, 0);
            ph.lineTo(32, 0);
            for (qreal w : {
                 1.0, 2.0, 3.0, 4.0, 5.0,
             }) {
                QString name = QVariant::fromValue(w).toString();
                ToolButton::Flags flags = nullptr;
                QGraphicsPathItem * item = new QGraphicsPathItem(ph);
                item->setPen(QPen(Qt::black, w * 3));
                QGraphicsItem * icon = item;
                ToolButton * btn = new ToolButton({name, name, flags,
                     QVariant::fromValue(icon)});
                lineWidthButtons.append(btn);
            }
        }
        buttons.append(lineWidthButtons);
    } else {
        static_cast<Geometry *>(res_)->getToolButtons(buttons, button);
    }
}

void GeometryControl::updateToolButton(ToolButton *button)
{
    if (button->name == "setColor(QColor)") {
        QGraphicsItem * icon = colorIcon(res_->property("color").value<QColor>());
        button->icon = QVariant::fromValue(icon);
    } else {
        static_cast<Geometry *>(res_)->updateToolButton(button);
    }
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
    if (editing_) {
        editing_ = false;
        GeometryItem * item = static_cast<GeometryItem *>(item_);
        item->showEditor(false);
    } else if (selected && res_->metaObject() == &Line::staticMetaObject) {
        WhiteCanvas * canvas = static_cast<WhiteCanvas *>(
                    realItem_->parentItem()->parentItem());
        canvas->selector()->selectImplied(realItem_);
        edit();
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
        updateGraph();
    else
        Control::updateTransform();
}

void GeometryControl::updateGraph()
{
    Geometry * geometry = qobject_cast<Geometry *>(res_);
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

void GeometryControl::finishGraph()
{
    bool hasFinished = flags_ & DrawFinished;
    flags_ |= DrawFinished;
    Geometry * geometry = static_cast<Geometry *>(res_);
    geometry->finish(bounds().center());
    updateTransform();
    updateGraph();
    WhiteCanvas * canvas = static_cast<WhiteCanvas *>(
                realItem_->parentItem()->parentItem());
    if (!hasFinished) {
        canvas->selector()->selectImplied(realItem_);
        edit();
    } else {
        if (canvas->selector()->selected() == realItem_)
            canvas->selector()->updateSelect();
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
            updateGraph();
            return true;
        }
        break;
    }
    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (flags_ & DrawFinished) {
            QPointF pt = me->pos() + hitOffset_;
            if (graph->move(hitElem_, pt)) {
                updateGraph();
            }
        } else {
            graph->movePoint(me->pos());
            updateGraph();
        }
        return true;
    }
    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (flags_ & DrawFinished) {
            finishGraph();
        } else {
            if (graph->commitPoint(me->pos())) {
                finishGraph();
            } else if (graph->canFinish()) {
                me->setFlags(static_cast<Qt::MouseEventFlags>(256));
                updateGraph();
            }
        }
        return true;
    }
    case QEvent::GraphicsSceneHoverMove:
        if (!(flags_ & DrawFinished)) {
            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
            if (graph->moveTempPoint(me->pos())) {
                 updateGraph();
                 return true;
            }
        }
        break;
    case QEvent::User:
        if (graph->canFinish()) {
            finishGraph();
        } else {
            WhiteCanvas * canvas = static_cast<WhiteCanvas *>(
                        realItem_->parentItem()->parentItem());
            canvas->removeResource(this);
        }

        break;
    default:
        break;
    }
    event->ignore();
    return false;
}
