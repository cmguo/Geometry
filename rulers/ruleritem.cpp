#include "ruleritem.h"
#include "rulertool.h"
#include "ruler.h"
#include "base/geometryhelper.h"
#include "base/geometrycontrol.h"
#include "base/geometry.h"

#include <core/control.h>
#include <core/resourceview.h>
#include <core/resourcetransform.h>

#include <QEvent>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QtMath>
#include <QDebug>

RulerItem::RulerItem(Ruler * ruler, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    GeometryHelper::init();
    ruler_ = ruler;
    deleteItem_ = iconItem(":/geometry/icon/ruler/delete.svg");
    adjustItem_ = iconItem(":/geometry/icon/ruler/adjust.svg");
    rotateItem_ = iconItem(":/geometry/icon/ruler/rotate.svg");
    setFiltersChildEvents(true);
    adjustControlPositions();
}

RulerItem::~RulerItem()
{
}

QRectF RulerItem::boundingRect() const
{
    return QRectF(0, 0, ruler_->width_, ruler_->height_);
}

QPainterPath RulerItem::shape() const
{
    return ruler_->shape_;
}

bool RulerItem::sceneEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress: {
        Geometry * geometry = ruler_->createGeometry();
        if (geometry == nullptr)
            break;
        geometry->transform() *= ruler_->transform().rotate();
        geometry->transform().translate(scenePos());
        RulerTool *control = qobject_cast<RulerTool*>(RulerTool::fromItem(this));
        geometry_ = control->addGeometry(geometry);
        geometry_->event(event);
        break;
    }
    case QEvent::GraphicsSceneMouseMove:
        if (geometry_ != nullptr) {
            geometry_->event(event);
        }
        break;
    case QEvent::GraphicsSceneMouseRelease:
        if (geometry_ != nullptr) {
            geometry_->event(event);
            RulerTool *control = qobject_cast<RulerTool*>(RulerTool::fromItem(this));
            control->finishGeometry(geometry_);
        }
        geometry_ = nullptr;
        break;
    default:
        return QGraphicsItem::sceneEvent(event);
        break;
    }
    return true;
}

bool RulerItem::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if (watched != deleteItem_ && watched != rotateItem_ && watched != adjustItem_){
        return false;
    }
    QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
    switch (event->type()) {
    case QEvent::GraphicsSceneHoverEnter:
        static_cast<QGraphicsPathItem*>(watched)->setBrush(Qt::gray);
        break;
    case QEvent::GraphicsSceneMousePress: {
        static_cast<QGraphicsPathItem*>(watched)->setBrush(Qt::gray);
        isPressed = true;
        QPointF mousePos = mouseEvent->scenePos();
        if (watched == adjustItem_)
            mousePos = mapFromScene(mousePos);
        lastPoint_ = mousePos;
        break;
    }
    case QEvent::GraphicsSceneMouseMove: {
        QPointF mousePos = mouseEvent->scenePos();
        RulerTool *control = qobject_cast<RulerTool*>(RulerTool::fromItem(this));
        if (watched == rotateItem_) {
            control->rotate(mapToParent(ruler_->rotateCenter_), lastPoint_, mousePos);
        } else if (watched == adjustItem_) {
            mousePos = mapFromScene(mousePos);
            QPointF offset = ruler_->adjust(mousePos - lastPoint_);
            offset = control->resource()->transform().rotate().map(offset);
            mousePos -= offset;
            control->move(offset);
            control->sizeChanged();
            adjustControlPositions();
        }
        lastPoint_ = mousePos;
    }
        break;
    case QEvent::GraphicsSceneMouseRelease:
        static_cast<QGraphicsPathItem*>(watched)->setBrush(QBrush());
        if (watched == deleteItem_) {
            ruler_->removeFromPage();
        }
        isPressed = false;
        break;
    case QEvent::GraphicsSceneHoverLeave:
        static_cast<QGraphicsPathItem*>(watched)->setBrush(QBrush());
        break;
    default:
        break;
    }
    return true;
}

void RulerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(250, 250, 250, 100));
    painter->drawPath(ruler_->shape1_);
    painter->setBrush(QColor(250, 250, 250, 160));
    painter->drawPath(ruler_->shape2_);
    painter->setPen(QPen(Qt::black, 1));
    painter->setRenderHints(QPainter::TextAntialiasing);
    painter->setFont(GeometryHelper::TEXT_FONT);
    ruler_->onDraw(painter);
}

void RulerItem::updateShape()
{
    prepareGeometryChange();
    ruler_->updateShape();
}

void RulerItem::adjustControlPositions()
{
    QVector<QPointF> points = ruler_->getControlPositions();
    deleteItem_->setPos(points[0]);
    adjustItem_->setPos(points[1]);
    rotateItem_->setPos(points[2]);
}

QGraphicsItem *RulerItem::iconItem(QString const & url)
{
    QPixmap icon(url);
    QGraphicsPathItem * back = new QGraphicsPathItem(this);
    back->setPen(QPen(Qt::darkGray, 2));
    int add = icon.width() / 4;
    QPainterPath path; path.addEllipse({{0, 0}, icon.size() + QSize(add, add) * 2});
    back->setPath(path);
    back->setAcceptedMouseButtons(Qt::LeftButton);
    back->setAcceptHoverEvents(true);
    back->setTransform(QTransform::fromTranslate(-icon.width() / 2 - add,
                                                 -icon.height() / 2 - add));
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem(back);
    item->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    item->setTransformationMode(Qt::SmoothTransformation);
    item->setPixmap(icon);
    item->setPos(add, add);
    return back;
}

