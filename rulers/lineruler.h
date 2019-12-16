#ifndef LINERULER_H
#define LINERULER_H

#include "rulergaugebase.h"


class Lineruler:public RulerGaugeBase
{
public:
    explicit Lineruler(QGraphicsItem *parent = nullptr);
    Lineruler(int width,int height,QGraphicsItem *parent = nullptr);
    ~Lineruler();
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void rotation() override;
    virtual void adjustSize() override;
};

#endif // LINERULER_H
