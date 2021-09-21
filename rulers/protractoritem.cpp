#include "protractoritem.h"

#include <QPainter>
#include <QtMath>
#include <geometryhelper.h>

ProtractorItem::ProtractorItem(QGraphicsItem *parent)
    : ProtractorItem(500, parent)
{
}

ProtractorItem::ProtractorItem(qreal width, QGraphicsItem *parent)
    : RulerItem(width, width / 2 + 20, parent)
{
    minWidth_ = 500;
    updateShape();
    adjustControlButtonPos();
}

ProtractorItem::~ProtractorItem()
{
}

QPointF ProtractorItem::adjustDirection(QRectF &adjust)
{
    adjust = {-1, -1, 2, 1};
    return {0, -1};
}

QVector<QPointF> ProtractorItem::getControlButtonPos()
{
    return QVector<QPointF>{
        {100, height_ - 40},
        {width_ / 2, 100},
        {width_ - 100, height_ - 40},
    };
}

void ProtractorItem::updateShape()
{
    QRectF cicle{0, 0, width_, width_};
    shape_ = QPainterPath();
    shape_.addEllipse(cicle);
    cicle.adjust(Unit * 4, Unit * 4, -Unit * 4, -Unit * 4);
    QPainterPath shape1;
    shape1.addEllipse(cicle);
    QPainterPath shape2;
    cicle.adjust(20, 20, -20, -20);
    shape2.addEllipse(cicle);
    QPainterPath shape3;
    cicle.adjust(20, 20, -20, -20);
    shape3.addEllipse(cicle);
    shape2_ = shape1 - shape2 + shape3;
    QPainterPath bounds;
    bounds.addRect(boundingRect());
    shape_ = shape_ & bounds;
    shape2_ = shape2_ & bounds;
    rotateCenter_ = {width_ / 2, width_ / 2};
    RulerItem::updateShape();
}

void ProtractorItem::onDraw(QPainter *painter)
{
    qreal r = width_ / 2;
    qreal r2 = width_ / 6;
    painter->setTransform(QTransform(0, -1, 1, 0, r, r), true);
    qreal one = M_PI / 180;
    QTransform d(cos(one), sin(one), -sin(one), cos(one), 0, 0);
    Qt::Alignment alignment = Qt::AlignTop | Qt::AlignHCenter;
    QPen pen = painter->pen();
    QPen pen2 = pen;
    pen2.setWidthF(0.5);
    for (int i = 0; i <= 180; ++i) {
        int l = 0;
        if (i % 10 == 0) { // per 10 degree
            l = 4;
        } else if (i % 5 == 0) { // pre 5 degree
            l = 3;
        } else {
            l = 2;
        }
        QPointF end{0, -r + Unit * l};
        painter->drawLine(QPointF{0, -r}, end);
        if (i % 10 == 0) {
            QString t = QString::number(i);
            QRectF txtRect = GeometryHelper::textRect(t, end, alignment);
            painter->drawText(txtRect, t);
            t = QString::number(180 - i);
            end.setY(end.y() + 20);
            txtRect = GeometryHelper::textRect(t, end, alignment);
            painter->drawText(txtRect, t);
            end.setY(end.y() + 20);
            if (i % 30 == 0) {
                painter->drawLine(end, {0, 0});
            } else {
                painter->setPen(pen2);
                painter->drawLine(end, {0, -r2});
                painter->setPen(pen);
            }
        }
        painter->setTransform(d, true);
    }
    QRectF cicle{-r2, -r2, r2 * 2, r2 * 2};
    painter->drawArc(cicle, 90 * 16, 180 * 16);
}

