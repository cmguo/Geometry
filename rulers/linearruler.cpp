#include "linearruler.h"

#include <QPainter>
#include <QPen>

LinearRuler::LinearRuler(Resource * res)
    : Ruler(res)
{
    width_ = 500;
    height_ = 100;
}

QVector<QPointF> LinearRuler::getControlPositions()
{
    return QVector<QPointF>{
        {40, 70},
        {width_ - 100, 70},
        {width_ - 40, 70},
    };
}

void LinearRuler::updateShape()
{
    QRectF rect{0, 0, width_, height_};
    shape_ = QPainterPath();
    shape_.addRoundedRect(rect, Unit, Unit);
    shape2_ = QPainterPath();
    shape2_.addRect(rect.adjusted(0, Unit * 3, 0, 0));
    Ruler::updateShape();
}

void LinearRuler::onDraw(QPainter *painter)
{
    QPointF corner{Unit / 2 * 5, Unit / 2};
    drawTickMarks(painter, corner, {width_ - corner.x(), corner.y()}, 0);
}
