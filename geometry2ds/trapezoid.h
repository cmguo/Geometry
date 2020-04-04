#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include "polygon.h"

class Trapezoid : public Polygon
{
    Q_OBJECT
public:
    Trapezoid(Resource * res);

    Trapezoid(Trapezoid const & o);

public:
    virtual void movePoint(QPointF const & pt) override;

    virtual bool commitPoint(QPointF const & pt) override;

    virtual int pointCount() override;

    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual QPointF point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;

protected:
    virtual QPointF point4(QPointF const & pt1, QPointF const & pt2, QPointF const & pt3) = 0;
};

#endif // TRAPEZOID_H
