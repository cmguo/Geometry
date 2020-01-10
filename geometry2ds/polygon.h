#ifndef POLYGON_H
#define POLYGON_H

#include "geometry2d.h"

class Polygon : public Geometry2D
{
    Q_OBJECT
public:
    Q_INVOKABLE Polygon(Resource * res, Flags flags = {});

    Polygon(QPointF const & pt);

    Polygon(Polygon const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;

public slots:
    void draw(QPainter *painter);

protected:
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

    void addAngleLabeling(QPainterPath & path, int index);

    void addAngleLabeling(QPainterPath & path, int index, qreal angle);

protected:
    using Geometry2D::addAngleLabeling;

private:
    QPainterPath path_;
    QPainterPath textPath_;
};

#endif // POLYGON_H
