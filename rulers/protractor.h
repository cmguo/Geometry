
#ifndef PROTRACTOR_H
#define PROTRACTOR_H

#include "rulergaugebase.h"

// 量角器
class Protractor:public RulerGaugeBase
{
public:
    explicit Protractor(QGraphicsItem *parent = nullptr);
    Protractor(int width,int height,QGraphicsItem *parent = nullptr);
    ~Protractor();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QPointF adjustSize(QPointF from,QPointF to) override;
    virtual QVector<QPointF> getControlButtonPos() override;
    virtual void updateShape();
};


#endif // PROTRACTOR_H
