#include "rulertriangle.h"
#include "triangleruler.h"

#include <core/resource.h>

#include <QPainter>
#include <QtMath>

static constexpr qreal SQRT2_2 = 0.70710678118654752440084436210485;
static constexpr qreal SQRT3_2 = 0.86602540378443864676372317075294;

TriangleRuler::TriangleRuler(Resource * res)
    : Ruler(res)
{
    isosceles_ = res->property(Resource::PROP_SUB_TYPE) == "iso_triangle";
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
    qreal offset = Unit * 8 + 40;
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
    qreal ratio1 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2);
    qreal ratio2 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2 + 2.0);
    shape_ = QPainterPath();
    shape_.addPolygon(QVector<QPointF>{
        {0, 0},
        {0, height_},
        {width_, height_},
    });
    qreal offset = Unit * 3;
    shape1_ = QPainterPath();
    shape1_.addPolygon(QVector<QPointF>{
        {offset, offset * ratio1},
        {offset, height_ - offset},
        {width_ - offset * ratio2, height_ - offset},
    });
    offset += Unit * 3;
    shape2_ = QPainterPath();
    shape2_.addPolygon(QVector<QPointF>{
        {offset, offset * ratio1},
        {offset, height_ - offset},
        {width_ - offset * ratio2, height_ - offset},
    });
    shape1_ -= shape2_;
}

void TriangleRuler::onDraw(QPainter *painter)
{
    qreal offset = Unit * 3;
    drawTickMarks(painter, {offset, height_ - offset}, {offset, offset},
                  CrossLittenEndian | NeedRotate | ClipByShape);
    drawTickMarks(painter, {offset, height_ - offset}, {width_ - offset, height_ - offset},
                  Anticlockwise | CrossLittenEndian | ClipByShape);
    qreal offset2 = Unit * 8;
    qreal ratio1 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2);
    qreal ratio2 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2 + 2.0);
    qreal ratio11 = isosceles_ ? (SQRT2_2 + 1.0) : (1.5);
    qreal ratio12 = isosceles_ ? (SQRT2_2 + 1.0) : (SQRT3_2);
    qreal ratio21 = isosceles_ ? (SQRT2_2 + 1.0) : (SQRT3_2 * 2 + 1.5);
    qreal ratio22 = isosceles_ ? (SQRT2_2 + 1.0) : (SQRT3_2 + 1.0);
    drawTickMarks(painter, {offset + offset2 * ratio11, offset * ratio1 + offset2 * ratio12},
                           {width_ - offset * ratio2 - offset2 * ratio21, height_ - offset - offset2 * ratio22},
                  NeedRotate | ClipByShape);
}

Geometry *TriangleRuler::createGeometry()
{
    qreal ratio1 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2);
    qreal ratio2 = isosceles_ ? (SQRT2_2 * 2 + 1.0) : (SQRT3_2 * 2 + 2.0);
    qreal offset = Unit * 3;
    return new RulerTriangle({
         {offset, offset * ratio1},
         {width_ - offset * ratio2, height_ - offset},
         {offset, height_ - offset},
     });
}
