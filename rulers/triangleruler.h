#ifndef TRIANGLERULER_H
#define TRIANGLERULER_H

#include "rulergaugebase.h"


class TriangleRuler:public RulerGaugeBase
{
public:
    explicit TriangleRuler(bool isosceles,QGraphicsItem *parent = nullptr);
    TriangleRuler(bool isosceles,int width,int height,QGraphicsItem *parent = nullptr);
    ~TriangleRuler();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void rotation() override;
    virtual void adjustSize() override;
private:
    bool isosceles_; //是否等腰三角尺
};

#endif // TRIANGLERULER_H
