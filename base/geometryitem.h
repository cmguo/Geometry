#ifndef GEOMETRY_ITEM_H
#define GEOMETRY_ITEM_H

#include "core/controlview.h"

#ifdef SHOWBOARD_QUICK
#include <QQuickItem>
#else
#include <QGraphicsPathItem>
#endif
#include <QMetaMethod>

class Geometry;

#ifdef SHOWBOARD_QUICK
class GeometryItem : public QQuickItem
#else
class GeometryItem : public QGraphicsPathItem
#endif
{
public:
    GeometryItem(Geometry * geometry, ControlView * parent = nullptr);

    void setEditPoints(QVector<QPointF> const & points);

    void showEditor(bool show);

    ControlView * editItem()
    {
        return editItem_;
    }

    void setColor(const QColor & color);

    void setContourPath(const QPainterPath & path);

private:
    virtual bool contains(const QPointF &point) const override;

#ifdef SHOWBOARD_QUICK

#else

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual bool sceneEvent(QEvent * event) override;

    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;

#endif

private:
    ControlView * editItem_;
    Geometry * geometry_;
    QMetaMethod methodContains_;
};

#endif // GEOMETRY_ITEM_H
