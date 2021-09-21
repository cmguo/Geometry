
#ifndef PROTRACTOR_H
#define PROTRACTOR_H

#include "ruleritem.h"

// 量角器
class ProtractorItem : public RulerItem
{
public:
    explicit ProtractorItem(QGraphicsItem *parent = nullptr);

    ProtractorItem(qreal width, qreal height, QGraphicsItem *parent = nullptr);

    ~ProtractorItem() override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF adjustDirection(QRectF &adjust) override;

    virtual QVector<QPointF> getControlButtonPos() override;

    virtual void updateShape() override;
};


#endif // PROTRACTOR_H
