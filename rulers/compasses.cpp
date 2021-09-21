#include "compasses.h"

Compasses::Compasses(Resource * res)
    : Ruler(res)
{
}

QPointF Compasses::adjustDirection(QRectF &)
{
    return {0, 1};
}

QVector<QPointF> Compasses::getControlPositions()
{
    QVector<QPointF> points;
    points.insert(0,QPointF(0,0));
    points.insert(1,QPointF(100,100));
    points.insert(2,QPointF(200,200));
    return points;
}

void Compasses::updateShape()
{
}
