#ifndef LINERULER_H
#define LINERULER_H

#include "ruleritem.h"

class LinearRulerItem : public RulerItem
{
public:
    explicit LinearRulerItem(QGraphicsItem *parent = nullptr);

    LinearRulerItem(qreal width, qreal height, QGraphicsItem *parent = nullptr);

    ~LinearRulerItem() override;

protected:
    virtual QVector<QPointF> getControlButtonPos() override;

    virtual void updateShape() override;

    void onDraw(QPainter *painter) override;
};

#endif // LINERULER_H
