#ifndef GEOMETRY_ITEM_H
#define GEOMETRY_ITEM_H

#include <QGraphicsPathItem>

class GeometryItem : public QGraphicsPathItem
{
public:
    GeometryItem(QGraphicsItem * parent = nullptr);

private:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual bool sceneEvent(QEvent * event) override;
};

#endif // GEOMETRY_ITEM_H
