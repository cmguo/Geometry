#ifndef LINEARRULER_H
#define LINEARRULER_H

#include "ruler.h"

class LinearRuler : public Ruler
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit LinearRuler(Resource * res);

protected:
    virtual QVector<QPointF> getControlPositions() override;

    virtual void updateShape() override;

    void onDraw(QPainter *painter) override;
};

#endif // LINEARRULER_H
