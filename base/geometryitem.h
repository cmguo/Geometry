#ifndef GEOMETRY_ITEM_H
#define GEOMETRY_ITEM_H

#include <QGraphicsPathItem>
#include <QMetaMethod>

class Geometry;

class GeometryItem : public QGraphicsPathItem
{
public:
    GeometryItem(Geometry * geometry, QGraphicsItem * parent = nullptr);

    void setEditPoints(QVector<QPointF> const & points);

    void showEditor(bool show);

    QGraphicsPathItem * editItem()
    {
        return editItem_;
    }

    void setPen(const QPen &pen);

private:
    virtual bool contains(const QPointF &point) const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual bool sceneEvent(QEvent * event) override;

    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;

private:
    QGraphicsPathItem * editItem_;
    Geometry * geometry_;
    QMetaMethod methodContains_;
};

#endif // GEOMETRY_ITEM_H