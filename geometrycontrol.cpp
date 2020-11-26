#include "geometrycontrol.h"
#include "geometryitem.h"
#include "geometry.h"
#include "geometry2ds/line.h"
#include "geometry2ds/circle.h"
#include "geometry3ds/geometry3d.h"

#include <core/toolbutton.h>
#include <core/resourcetransform.h>
#include <core/optiontoolbuttons.h>
#include <views/whitecanvas.h>
#include <views/itemselector.h>
#include <quick/quickhelper.h>

#include <QFile>
#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QTouchEvent>
#include <QPen>
#include <QQuickWidget>
#include <QQuickItem>

#include <float.h>

static char const * toolstr =
        "edit()|调节|HideSelector|:/showboard/icon/edit.svg;"
        "|;"
        "color|颜色|Popup,OptionsGroup,NeedUpdate|;"
        #ifdef QT_DEBUG
        "width|线宽|Popup,OptionsGroup,NeedUpdate|;"
        "test()|测试|;"
        #endif
        ;

GeometryControl::GeometryControl(ResourceView * res, Flags flags, Flags clearFlags)
    : Control(res, flags | KeepAspectRatio | LayoutScale | Touchable | ShowSelectMask, clearFlags)
    , hitElem_(-1)
    , hitMoved_(false)
    , editing_(false)
    , touchId_(0)
{
#ifdef QT_DEBUG
    flags_ |= ImpliedEditable;
#else
    flags_ |= HalfSelect;
#endif
    if (res_->metaObject() == &Line::staticMetaObject)
        flags_ &= ~CanScale;
    if (res->metaObject() == &Circle::staticMetaObject
            || res->metaObject()->inherits(&Geometry3D::staticMetaObject))
        flags_ &= ~CanRotate;
    setMinSize({0.02, 0.02});
}

ControlView *GeometryControl::create(ControlView *parent)
{
    (void) parent;
    GeometryItem * item = new GeometryItem(qobject_cast<Geometry*>(res_));
    //item->setBrush(QColor(0, 0, 255, 20));
    item->editItem()->setData(1000 /*ITEM_KEY_CONTROL*/, QVariant::fromValue(this));
    return item;
}

void GeometryControl::attaching()
{
    Geometry * geometry = static_cast<Geometry *>(res_);
    if (geometry->empty())
        geometry->init();
}

void GeometryControl::attached()
{
    Geometry * geometry = static_cast<Geometry *>(res_);
    updateSettings();
    QObject::connect(geometry, &Geometry::changed,
                     this, &GeometryControl::geometryChanged);
    if (geometry->empty() || (flags_ & RestoreSession)) {
        if (flags_ & RestoreSession) {
            updateGeometry();
        } else {
            flags_ &= ~CanSelect;
            whiteCanvas()->topControl()->setProperty("editingMode", 0);
        }
        loadFinished(true);
    } else {
        auto life(this->life());
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

QString GeometryControl::toolsString(QByteArray const & parent) const
{
    if (parent.isEmpty()) {
        return toolstr;
    }
    return nullptr;
}

static ColorToolButtons colorButtons({Qt::white, Qt::black, Qt::red,
                                      Qt::green, Qt::blue, Qt::yellow});
REGISTER_OPTION_BUTTONS(GeometryControl, color, colorButtons)

static WidthToolButtons widthButtons({1.0, 2.0, 3.0, 4.0, 5.0});
REGISTER_OPTION_BUTTONS(GeometryControl, width, widthButtons)

void GeometryControl::getToolButtons(QList<ToolButton *> &buttons, const QList<ToolButton *> &parents)
{
    Control::getToolButtons(buttons, parents);
    if (parents.isEmpty()) {
        int i = buttons.indexOf(&ToolButton::SPLITTER);
        while (i > 0 && buttons[i - 1]->name() != "edit()") {
            i = buttons.indexOf(&ToolButton::SPLITTER, i + 1);
        }
        if (i > 2) {
            buttons.prepend(buttons.takeAt(i));
            buttons.prepend(buttons.takeAt(i));
        }
    }
}

bool GeometryControl::setOption(const QByteArray &key, QVariant value)
{
    // set to geometry
    return res_->setOption(key, value);
}

QVariant GeometryControl::getOption(const QByteArray &key)
{
    return res_->property(key);
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

void GeometryControl::test()
{
    QQuickWidget * widget = new QQuickWidget();
    QSurfaceFormat fmt;
    fmt.setSamples(4);
    widget->setFormat(fmt);
    widget->setSource(QUrl("qrc:/geometry/qml/GeometryShape.qml"));
    widget->setClearColor(Qt::transparent);
    Geometry * geometry = qobject_cast<Geometry *>(res_);
    QQuickItem * root = widget->rootObject();
    QuickHelper::appendChild(root, geometry->toQuickPath(root));
    QRectF rect = item_->boundingRect();
    root->setProperty("width", rect.width());
    root->setProperty("height", rect.height());
    root->setProperty("x", -rect.left());
    root->setProperty("y", -rect.top());
    widget->show();
}

void GeometryControl::updateSettings()
{
    Geometry * geometry = qobject_cast<Geometry *>(res_);
    setPen(QPen(geometry->color(), geometry->width()));
}

void GeometryControl::geometryChanged(const QByteArray &key)
{
    finishGeometry();
    if (key == "color" || key == "width")
        updateSettings();
}

Control::SelectMode GeometryControl::selectTest(QPointF const & point)
{
    QPointF pt = point;
    GeometryItem * item = static_cast<GeometryItem *>(item_);
    Geometry * geometry = qobject_cast<Geometry *>(res_);
    return (editing_ && item->editItem()->contains(point))
            || geometry->hit(pt) >= 0 ? NotSelect : Select;
}

void GeometryControl::select(bool selected)
{
    if (editing_) {
        editing_ = false;
        GeometryItem * item = static_cast<GeometryItem *>(item_);
        item->showEditor(false);
    //} else if (selected && res_->metaObject() == &Line::staticMetaObject) {
    //    whiteCanvas()->selector()->selectImplied(this);
    //    edit();
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
    geometry->sync();
    item->setPath(geometry->contour());
    if (editing_) {
        editPoints_ = geometry->movePoints();
        item->setEditPoints(editPoints_);
    }
}

void GeometryControl::finishGeometry(bool valid)
{
    Geometry * geometry = static_cast<Geometry *>(res_);
    GeometryItem * item = static_cast<GeometryItem *>(item_);
    geometry->sync();
    if (!valid) {
        item->setPath(geometry->contour());
    }
    bool hasFinished = geometry->finished();
    QRectF bound = item->boundingRect();
    geometry->finish(bound.center());
    updateGeometry();
    item->update(); // contour may not change
    ItemSelector * selector = whiteCanvas()->selector();
    if (!hasFinished) {
        flags_ |= CanSelect;
        res_->setProperty("originSize", bound.size());
        selector->selectImplied(this);
        edit();
    } else {
        res_->setProperty("originSize", bound.size() / res_->transform().zoom());
        selector->updateSelect(this);
    }
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
                geometry->beginMove(hitElem_);
                return true;
            }
        } else if (!editing_ && (flags_ & ImpliedEditable)) {
            hitElem_ = geometry->hit(pt);
            if (hitElem_ >= 0) {
                qDebug() << "hit" << hitElem_;
                hitOffset_ = pt - point;
                geometry->beginMove(hitElem_);
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
        if (geometry->moveElememt(hitElem_, pt)) {
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
            geometry->endMove(hitElem_);
        } else {
            whiteCanvas()->selector()->select(this);
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
    //qDebug() << "GeometryControl::event" << event->type();
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
                break;
            }
        }
        return true;
    }
    case QEvent::TouchEnd: {
        QTouchEvent * te = static_cast<QTouchEvent *>(event);
        for (QTouchEvent::TouchPoint const & p : te->touchPoints()) {
            if (p.id() == touchId_) {
                touchPos_ = p.pos();
                break;
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
        }
        break;
    default:
        break;
    }
    event->ignore();
    return false;
}
