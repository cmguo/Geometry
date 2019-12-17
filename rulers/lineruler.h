#ifndef LINERULER_H
#define LINERULER_H

#include "rulergaugebase.h"


class Lineruler:public RulerGaugeBase
{
public:
    explicit Lineruler(QGraphicsItem *parent = nullptr);
    Lineruler(int width,int height,QGraphicsItem *parent = nullptr);
    ~Lineruler();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QPointF adjustSize(QPointF from,QPointF to) override;
    virtual QVector<QPointF> getControlButtonPos() override;
    virtual void updateShape();
};

#endif // LINERULER_H
