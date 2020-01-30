#include "geometrycontrol.h"
#include "geometryitem.h"
#include "geometry.h"
#include "geometry2ds/line.h"
#include "geometry2ds/regularpolygon.h"
#include "geometry2ds/sector.h"
#include "geometry2ds/circle.h"

#include <core/toolbutton.h>
#include <core/resourcetransform.h>
#include <views/whitecanvas.h>
#include <views/itemselector.h>

#include <QFile>
#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QTouchEvent>
#include <QPen>

#include <float.h>

static char const * toolstr =
        "edit()|调节|HideSelector|:/showboard/icons/edit.svg;"
        "setColor(QColor)|颜色|Popup,OptionsGroup,NeedUpdate|;"
        "-setLineWidth(qreal)|线宽|Popup,OptionsGroup,NeedUpdate|;";

GeometryControl::GeometryControl(ResourceView * res, Flags flags, Flags clearFlags)
    : Control(res, flags | KeepAspectRatio | LayoutScale | Touchable, clearFlags)
    , hitElem_(-1)
    , hitMoved_(false)
    , editing_(false)
    , touchId_(0)
{
    if (res_->metaObject() == &Line::staticMetaObject)
        flags_ &= ~(CanScale | CanRotate);
    if (res->metaObject() == &Circle::staticMetaObject)
        flags_ &= ~CanRotate;
}

QGraphicsItem * GeometryControl::create(ResourceView * res)
{
    (void) res;
    GeometryItem * item = new GeometryItem();
    //item->setBrush(QColor(0, 0, 255, 20));
    item->editItem()->setData(1000 /*ITEM_KEY_CONTROL*/, QVariant::fromValue(this));
    return item;
}

void GeometryControl::attached()
{
    updateSettings();
    Geometry * geometry = static_cast<Geometry *>(res_);
    QObject::connect(geometry, &Geometry::changed,
                     this, &GeometryControl::geometryChanged);
    if (geometry->empty() || (flags_ & RestoreSession)) {
        if (flags_ & RestoreSession)
            updateGeometry();
        loadFinished(true);
    } else {
        QWeakPointer<int> life(this->life());
        geometry->load().then([this, life]() {
            if (life.isNull())
                return;
            updateGeometry();
            loadFinished(true);
        });
    }
}

void GeometryControl::resize(const QSizeF &size)
{
    if (!(flags_ & LoadFinished))
        return;
    GeometryItem * item = static_cast<GeometryItem *>(item_);
    qreal scale = size.width() / item->boundingRect().width();
    if (qFuzzyIsNull(scale - 1.0))
        return;
    Geometry * geometry = static_cast<Geometry *>(res_);
    geometry->scale(scale);
    updateGeometry();
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
        static_cast<Geometry *>(res_)->getToolButtons(buttons, parents);
        Control::getToolButtons(buttons, parents);
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
                if (colorButtons.size() % 4 == 3)
                    colorButtons.append(&ToolButton::LINE_BREAK);
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
        static_cast<Geometry *>(res_)->getToolButtons(buttons, parents);
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

void GeometryControl::geometryChanged()
{
    finishGeometry();
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
        whiteCanvas()->selector()->selectImplied(realItem_);
        edit();
    }
    Control::select(selected);
}

void GeometryControl::setPen(const QPen &pen)
{
    GeometryItem * item = static_cast<GeometryItem *>(item_);
    item->setPen(pen);
}

void GeometryControl::updateGeometry()
{
    Geometry * geometry = qobject_cast<Geometry *>(res_);
    GeometryItem * item = static_cast<GeometryItem *>(item_);
    QPainterPath ph(geometry->path());
    item->setPath(ph);
    if (editing_) {
        editPoints_ = geometry->movePoints();
        item->setEditPoints(editPoints_);
    }
}

void GeometryControl::finishGeometry(bool valid)
{
    Geometry * geometry = static_cast<Geometry *>(res_);
    if (!valid) {
        static_cast<GeometryItem *>(item_)->setPath(geometry->path());
    }
    bool hasFinished = geometry->finished();
    if (geometry->flags() & ResourceView::DrawFinised)
        geometry->finish(bounds().center());
    else
        geometry->finish(bounds().center());
    updateGeometry();
    ItemSelector * selector = whiteCanvas()->selector();
    if (!hasFinished) {
        selector->selectImplied(realItem_);
        edit();
    } else {
        selector->updateSelect(realItem_);
    }
}

QRectF GeometryControl::bounds()
{
    return item_->boundingRect();
}

bool GeometryControl::beginPoint(const QPointF &point, bool fromHandle)
{
    Geometry * geometry = static_cast<Geometry *>(resource());
    hitStart_ = point;
    hitMoved_ = false;
    if (geometry->finished()) {
        QPointF pt = hitStart_;
        if (editing_ && fromHandle) {
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
            hitMoved_ = true;
            if (hitElem_ >= 0) {
                qDebug() << "hit handle" << hitElem_;
                hitOffset_ = editPoints_[mdx] - point;
                return true;
            }
        } else if (!editing_ && (flags_ & ImpliedEditable)) {
            hitElem_ = geometry->hit(pt);
            if (hitElem_ >= 0) {
                qDebug() << "hit" << hitElem_;
                hitOffset_ = pt - point;
                return true;
            }
        }
    } else {
        geometry->addPoint(point);
        updateGeometry();
        return true;
    }
    return false;
}

void GeometryControl::movePoint(const QPointF &point)
{
    Geometry * geometry = static_cast<Geometry *>(resource());
    if (!hitMoved_) {
        QPointF d = point - hitStart_;
        if (qAbs(d.x()) + qAbs(d.y()) < 5)
            return;
        hitMoved_ = true;
    }
    if (geometry->finished()) {
        QPointF pt = point + hitOffset_;
        if (geometry->move(hitElem_, pt)) {
            updateGeometry();
        }
    } else {
        geometry->movePoint(point);
        updateGeometry();
    }
}

bool GeometryControl::endPoint(const QPointF &point)
{
    Geometry * geometry = static_cast<Geometry *>(resource());
    bool canFinish = false;
    if (geometry->finished()) {
        if (hitMoved_) {
            finishGeometry(true);
        } else {
            whiteCanvas()->selector()->select(item());
        }
    } else {
        if (geometry->commitPoint(point)) {
            finishGeometry();
        } else if (geometry->canFinish()) {
            canFinish = true;
            updateGeometry();
        }
    }
    return canFinish;
}

bool GeometryControl::event(QEvent *event)
{
    Geometry * geometry = static_cast<Geometry *>(resource());
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress: {
        if (touchId_)
            return true;
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (beginPoint(me->pos(), me->flags() & 512)) // from GeometryItem
            return true;
        break;
    }
    case QEvent::GraphicsSceneMouseMove: {
        if (touchId_)
            return true;
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        movePoint(me->pos());
        return true;
    }
    case QEvent::GraphicsSceneMouseRelease: {
        if (touchId_)
            return true;
        QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (endPoint(me->pos()))
            me->setFlags(static_cast<Qt::MouseEventFlags>(256));
        return true;
    }
    case QEvent::TouchBegin: {
        QTouchEvent * te = static_cast<QTouchEvent *>(event);
        touchId_ = te->touchPoints().first().id();
        touchPos_ = te->touchPoints().first().pos();
        if (beginPoint(touchPos_, te->touchPointStates() & 512)) { // from GeometryItem
            return true;
        }
        break;
    }
    case QEvent::TouchUpdate: {
        QTouchEvent * te = static_cast<QTouchEvent *>(event);
        for (QTouchEvent::TouchPoint const & p : te->touchPoints()) {
            if (p.id() == touchId_) {
                movePoint(p.pos());
            }
        }
        return true;
    }
    case QEvent::TouchEnd: {
        QTouchEvent * te = static_cast<QTouchEvent *>(event);
        for (QTouchEvent::TouchPoint const & p : te->touchPoints()) {
            if (p.id() == touchId_) {
                touchPos_ = p.pos();
            }
        }
        touchId_ = 0;
        if (endPoint(touchPos_))
            te->setTouchPointStates(static_cast<Qt::TouchPointState>(256));
        return true;
    }
    case QEvent::GraphicsSceneHoverMove:
        if (!geometry->finished()) {
            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent *>(event);
            if (geometry->moveTempPoint(me->pos())) {
                 updateGeometry();
                 return true;
            }
        }
        break;
    case QEvent::User:
        if (geometry->canFinish()) {
            finishGeometry();
        } else {
            whiteCanvas()->removeResource(this);
        }

        break;
    default:
        break;
    }
    event->ignore();
    return false;
}
