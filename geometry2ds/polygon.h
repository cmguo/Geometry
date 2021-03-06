#ifndef POLYGON_H
#define POLYGON_H

#include "geometry2d.h"

class Polygon : public Geometry2D
{
    Q_OBJECT

#ifdef QT_DEBUG
    Q_CLASSINFO("toolsString", "radius|圆角|Popup,OptionsGroup,NeedUpdate|;")
#endif

    Q_PROPERTY(qreal radius MEMBER radius_)
public:
    Q_INVOKABLE Polygon(Resource * res, Flags flags = {});

    Polygon(Polygon const & o);

    Polygon(QPolygonF const &  polygon);

    Polygon(QVector<QPointF> const & points);

public:
    virtual QPainterPath graphPath() override;

    virtual QPainterPath textPath() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool moveElememt(int elem, const QPointF &pt) override;

    virtual void sync() override;

protected:
    virtual bool canClose();

    virtual int pointCount();

    virtual QPointF point(int index);

    virtual bool setPoint(int index, QPointF const & pt);

    virtual QPointF lastPoint(QPointF & hint);

    virtual QPointF firstPoint(QPointF & hint);

    virtual QPointF iterPoint(int index, QPointF & hint);

    virtual QPointF prevPoint(int index, QPointF & hint);

    virtual QPointF nextPoint(int index, QPointF & hint);

protected:
    bool moveKeepAngle(int elem, const QPointF &pt);

    qreal angle(int index);

    void addAngleLabeling(QPainterPath & graphPath, QPainterPath &textPath, int index);

protected:
    using Geometry2D::addAngleLabeling;

private:
    QPainterPath graphPath_;
    QPainterPath textPath_;
    qreal radius_;
};

#endif // POLYGON_H
