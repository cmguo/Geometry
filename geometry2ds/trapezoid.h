#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include "polygon.h"

class Trapezoid : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE Trapezoid(Resource * res);

    Trapezoid(QPointF const & pt);

    Q_INVOKABLE Trapezoid(Trapezoid const & o);

public:
    virtual int pointCount() override;

    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual QPointF point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // TRAPEZOID_H
