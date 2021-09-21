#ifndef TRIANGLERULER_H
#define TRIANGLERULER_H

#include "ruleritem.h"

class TriangleRulerItem:public RulerItem
{
public:
    explicit TriangleRulerItem(bool isosceles, QGraphicsItem *parent = nullptr);

    TriangleRulerItem(bool isosceles, qreal width, QGraphicsItem *parent = nullptr);

    ~TriangleRulerItem() override;

protected:
    QPointF adjustDirection(QRectF &adjust) override;

    virtual QVector<QPointF> getControlButtonPos() override;

    virtual void updateShape() override;

    void onDraw(QPainter *painter) override;

private:
    bool isosceles_; //是否等腰三角尺
};

#endif // TRIANGLERULER_H
