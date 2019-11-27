#ifndef RIGHTTRAPEZOID_H
#define RIGHTTRAPEZOID_H

#include "trapezoid.h"

class RightTrapezoid : public Trapezoid
{
    Q_OBJECT
public:
    Q_INVOKABLE RightTrapezoid(Resource * res);

    RightTrapezoid(QPointF const & pt);

    Q_INVOKABLE RightTrapezoid(RightTrapezoid const & o);

public:
    virtual QPointF point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // RIGHTTRAPEZOID_H
