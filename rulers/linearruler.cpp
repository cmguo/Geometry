#include "linearruler.h"
#include "rulerline.h"

LinearRuler::LinearRuler(Resource * res)
    : Ruler(res)
{
    width_ = 500;
    height_ = 115;
}

QVector<QPointF> LinearRuler::getControlPositions()
{
    return QVector<QPointF>{
        {40, 85},
        {width_ - 100, 85},
        {width_ - 40, 85},
    };
}

void LinearRuler::updateShape()
{
    shape_ = QPainterPath();
    shape_.addRoundedRect({0, 0, width_, height_}, Unit, Unit);
    shape1_ = QPainterPath();
    shape1_.addRect({0, Unit * 3, width_, Unit * 3});
    shape2_ = QPainterPath();
    shape2_.addRect({0, Unit * 6, width_, height_ - Unit * 6});
    shape2_ = shape2_ & shape_;
}

void LinearRuler::onDraw(QPainter *painter)
{
    QPointF corner{Unit / 2 * 5, Unit * 7 / 2};
    drawTickMarks(painter, corner, {width_ - corner.x(), corner.y()}, 0);
}

Geometry *LinearRuler::createGeometry()
{
    return new RulerLine(QLineF(0, 0, width_, 0).translated(0, Unit * 3));
}
