#ifndef COMPASSES_H
#define COMPASSES_H

#include "rulergaugebase.h"

class Compasses:public RulerGaugeBase
{
public:
    explicit Compasses(QGraphicsItem *parent = nullptr);
    ~Compasses();
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void rotation() override;
    virtual void adjustSize() override;
};

#endif // COMPASSES_H
