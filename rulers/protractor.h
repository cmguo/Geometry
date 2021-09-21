
#ifndef PROTRACTOR_H
#define PROTRACTOR_H

#include "ruler.h"

// 量角器
class Protractor : public Ruler
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit Protractor(Resource * res);

protected:
    virtual QPointF adjustDirection(QRectF &adjust) override;

    virtual QVector<QPointF> getControlPositions() override;

    virtual void updateShape() override;

    virtual void onDraw(QPainter *painter) override;
};


#endif // PROTRACTOR_H
