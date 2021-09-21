#ifndef COMPASSES_H
#define COMPASSES_H

#include "ruler.h"

// 圆规
class Compasses : public Ruler
{
public:
    explicit Compasses(Resource * res);

protected:
    virtual QPointF adjustDirection(QRectF &adjust) override;

    virtual QVector<QPointF> getControlPositions() override;

    virtual void updateShape() override;
};

#endif // COMPASSES_H
