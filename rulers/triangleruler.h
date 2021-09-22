#ifndef TRIANGLERULER_H
#define TRIANGLERULER_H

#include "ruler.h"

class TriangleRuler : public Ruler
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit TriangleRuler(Resource * res);

protected:
    virtual QPointF adjustDirection(QRectF &adjust) override;

    virtual QVector<QPointF> getControlPositions() override;

    virtual void updateShape() override;

    virtual void onDraw(QPainter *painter) override;

    virtual  Geometry * createGeometry() override;

private:
    bool isosceles_; //是否等腰三角尺
};

#endif // TRIANGLERULER_H
