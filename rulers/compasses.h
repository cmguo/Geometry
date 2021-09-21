#ifndef COMPASSES_H
#define COMPASSES_H

#include "ruleritem.h"

// 圆规
class Compasses : public RulerItem
{
public:
    explicit Compasses(QGraphicsItem *parent = nullptr);

    ~Compasses() override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF adjustDirection(QRectF &adjust) override;

    virtual QVector<QPointF> getControlButtonPos() override;

    virtual void updateShape() override;
};

#endif // COMPASSES_H
