#include "triangleruler.h"

#include <core/resource.h>

#include <QPainter>
#include <QtMath>

static constexpr qreal SQRT2_2 = 0.70710678118654752440084436210485;
static constexpr qreal SQRT3_2 = 0.86602540378443864676372317075294;

TriangleRuler::TriangleRuler(Resource * res)
    : Ruler(res)
{
    isosceles_ = res->type() == "iso_triangle";
    width_ = 500;
    height_ = isosceles_ ? (width_) : (width_ / (SQRT3_2 * 2));
    minWidth_ = isosceles_ ? 300 : 400;
}

QPointF TriangleRuler::adjustDirection(QRectF &adjust)
{
    if (isosceles_) {
        adjust = {0, -SQRT2_2 * 2, SQRT2_2 * 2, SQRT2_2 * 2};
        return {SQRT2_2, -SQRT2_2};
    } else {
        adjust = {0, -1, SQRT3_2 * 2, 1};
        return {0.5, -SQRT3_2};
    }
}

QVector<QPointF> TriangleRuler::getControlPositions()
{
    qreal offset = Unit * 5 + 40;
    qreal ratio1 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2);
    qreal ratio2 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2 + 2.0);
    return QVector<QPointF>{
        {offset, offset * ratio1},
        {(width_ - offset * (ratio2 - 1)) / 2, (height_ + offset * (ratio1 - 1)) / 2},
        {width_ - offset * ratio2, height_ - offset},
    };
}

void TriangleRuler::updateShape()
{
    shape_ = QPainterPath();
    QVector<QPointF> vector = {
        {0, 0},
        {0, height_},
        {width_, height_},
    };
    shape_.addPolygon(vector);
    qreal offset = Unit * 3;
    qreal ratio1 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2);
    qreal ratio2 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2 + 2.0);
    QVector<QPointF> vector2 = {
        {offset, offset * ratio1},
        {offset, height_ - offset},
        {width_- offset * ratio2, height_ - offset},
    };
    shape2_ = QPainterPath();
    shape2_.addPolygon(vector2);
    Ruler::updateShape();
}

void TriangleRuler::onDraw(QPainter *painter)
{
    drawTickMarks(painter, {0, height_}, {0, 0},
                  CrossLittenEndian | NeedRotate | ClipByShape);
    drawTickMarks(painter, {0, height_}, {width_, height_},
                  Anticlockwise | CrossLittenEndian | ClipByShape);
    qreal offset = Unit * 8;
    qreal ratio11 = isosceles_ ? (SQRT2_2 + 1.0) : (1.5);
    qreal ratio12 = isosceles_ ? (SQRT2_2 + 1.0) : (SQRT3_2);
    qreal ratio21 = isosceles_ ? (SQRT2_2 + 1.0) : (SQRT3_2 * 2 + 1.5);
    qreal ratio22 = isosceles_ ? (SQRT2_2 + 1.0) : (SQRT3_2 + 1.0);
    drawTickMarks(painter, {offset * ratio11, offset * ratio12},
                           {width_- offset * ratio21, height_ - offset * ratio22},
                  NeedRotate | ClipByShape);
}
