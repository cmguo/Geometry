
#ifndef PROTRACTOR_H
#define PROTRACTOR_H

#include "ruleritem.h"

// 量角器
class ProtractorItem : public RulerItem
{
public:
    explicit ProtractorItem(QGraphicsItem *parent = nullptr);

    ProtractorItem(qreal width, QGraphicsItem *parent = nullptr);

    ~ProtractorItem() override;

protected:
    QPointF adjustDirection(QRectF &adjust) override;

    virtual QVector<QPointF> getControlButtonPos() override;

    virtual void updateShape() override;

    void onDraw(QPainter *painter) override;
};


#endif // PROTRACTOR_H
