#ifndef COMPASSES_H
#define COMPASSES_H

#include "rulergaugebase.h"

class Compasses:public RulerGaugeBase
{
public:
    explicit Compasses(QGraphicsItem *parent = nullptr);
    ~Compasses();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QPointF adjustSize(QPointF from,QPointF to) override;
    virtual QVector<QPointF> getControlButtonPos() override;
    virtual void updateShape();
};

#endif // COMPASSES_H
