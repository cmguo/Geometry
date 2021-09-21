#include "ruleritem.h"
#include "geometryhelper.h"
#include "rulertool.h"

#include <core/control.h>
#include <core/resourceview.h>
#include <core/resourcetransform.h>

#include <QEvent>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QtMath>
#include <QDebug>

RulerItem::RulerItem(QGraphicsItem *parent)
    : RulerItem(500, 500, parent)
{
}

RulerItem::RulerItem(qreal width, qreal height,QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , width_(width)
    , height_(height)
{
    GeometryHelper::init();
    deleteItem_ = iconItem(":/geometry/icon/ruler/delete.svg");
    adjustItem_ = iconItem(":/geometry/icon/ruler/adjust.svg");
    rotateItem_ = iconItem(":/geometry/icon/ruler/rotate.svg");
}

RulerItem::~RulerItem()
{
}

QRectF RulerItem::boundingRect() const
{
    return QRectF(0, 0, width_, height_);
}

QPainterPath RulerItem::shape() const
{
    return shape_;
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
            control->rotate(mapToParent(rotateCenter_), lastPoint_, mousePos);
        } else if (watched == adjustItem_) {
            mousePos = mapFromScene(mousePos);
            QPointF offset = adjust(mousePos - lastPoint_);
            offset = control->resource()->transform().rotate().map(offset);
            mousePos -= offset;
            control->move(offset);
            control->sizeChanged();
            adjustControlButtonPos();
        }
        lastPoint_ = mousePos;
    }
        break;
    case QEvent::GraphicsSceneMouseRelease:
        static_cast<QGraphicsPathItem*>(watched)->setBrush(QBrush());
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

QVariant RulerItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemSceneHasChanged:
        if (!value.isNull()) {
            deleteItem_->installSceneEventFilter(this);
            adjustItem_->installSceneEventFilter(this);
            rotateItem_->installSceneEventFilter(this);
            updateShape();
            adjustControlButtonPos();
        }
        break;
    default:
        break;
    }
    return value;
}

void RulerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(250, 250, 250, 100));
    painter->drawPath(shape1_);
    painter->setBrush(QColor(250, 250, 250, 160));
    painter->drawPath(shape2_);
    painter->setPen(QPen(Qt::black, 1));
    painter->setRenderHints(QPainter::TextAntialiasing);
    painter->setFont(GeometryHelper::TEXT_FONT);
    onDraw(painter);
}

QVector<QPointF> RulerItem::tickMarkPoints(QPointF const & from, QPointF const & to, int flags)
{
    qreal length = GeometryHelper::length(to - from);
    QPointF delta = to - from;
    GeometryHelper::adjustToLength(delta, Unit);
    QPointF scale = delta;
    if (flags & Anticlockwise)
        GeometryHelper::rotate(scale, QPointF(0, -1));
    else
        GeometryHelper::rotate(scale, QPointF(0, 1));
    QVector<QPointF> points;
    QPointF cur = from;
    while (length > 0) {
        int l = 0;
        if ((flags & CrossLittenEndian) && points.size() < 10) {
            l = points.size() / 2;
        } else if (points.size() % 20 == 0) { // per 10mm
            l = 4;
        } else if (points.size() % 10 == 0) { // pre 5mm
            l = 3;
        } else {
            l = 2;
        }
        QPointF end = cur + scale * l;
        if ((flags & ClipByShape) && l > 0 && !shape_.contains(end))
            break;
        points.append(cur);
        points.append(end);
        length -= Unit;
        cur = cur + delta;
    }
    return points;
}

void RulerItem::drawTickMarks(QPainter *painter, const QPointF &from, const QPointF &to, int flags)
{
    if (flags & NeedRotate) {
        QPointF d = to - from;
        if (!qFuzzyIsNull(d.y())) {
            GeometryHelper::adjustToLength(d, 1.0);
            QTransform t = QTransform(d.x(), d.y(), -d.y(), d.x(), 0, 0)
                    * QTransform::fromTranslate(from.x(), from.y());
            QTransform t2 = t.inverted();
            QTransform o = painter->transform();
            painter->setTransform(t, true);
            if (flags & ClipByShape) {
                QPainterPath shape = shape_;
                shape_ = t2.map(shape);
                drawTickMarks(painter, {0, 0}, t2.map(to), flags & ~NeedRotate);
                shape_ = shape;
            } else {
                drawTickMarks(painter, {0, 0}, t2.map(to), flags & ~NeedRotate);
            }
            painter->setTransform(o);
            return;
        }
    }
    auto marks = tickMarkPoints(from, to, flags);
    painter->drawLines(marks);
    Qt::Alignment alignment = (flags & Anticlockwise)
            ? Qt::AlignBottom | Qt::AlignHCenter : Qt::AlignTop | Qt::AlignHCenter;
    for (int i = 0; i < marks.size(); i += 2) {
        if (i == 0 && (flags & CrossLittenEndian))
            continue;
        if (i % 20 == 0) {
            QString mark = QString::number(i / 20);
            QRectF txtRect = GeometryHelper::textRect(
                        mark, marks[i + 1], alignment);
            if (!shape_.contains(txtRect))
                break;
            painter->drawText(txtRect, mark);
        }
    }
}

QPointF RulerItem::adjust(QPointF const & offset)
{
    qDebug() << offset;
    QRectF adj;
    QPointF dir = adjustDirection(adj);
    qreal length = GeometryHelper::dotProduct(offset, dir);
    if (width_ + adj.width() * length < minWidth_)
        length = (minWidth_ - width_) / adj.width();
    QPointF topLeft = adj.topLeft() * length;
    QSizeF size = adj.size() * length;
    qDebug() << length << topLeft << size;
    lastPoint_ += topLeft;
    width_ += size.width();
    height_ += size.height();
    prepareGeometryChange();
    updateShape();
    return topLeft;
}

QPointF RulerItem::adjustDirection(QRectF &adjust)
{
    adjust.setRight(1);
    return QPointF(1, 0);
}

void RulerItem::updateShape()
{
    shape1_ = shape_ - shape2_;
    shape2_ = shape_ & shape2_;
}

void RulerItem::onDraw(QPainter *painter)
{
    (void) painter;
}

void RulerItem::adjustControlButtonPos()
{
    QVector<QPointF> points = getControlButtonPos();
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

