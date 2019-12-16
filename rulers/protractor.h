
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

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void rotation() override;
    virtual void adjustSize() override;
};


#endif // PROTRACTOR_H
