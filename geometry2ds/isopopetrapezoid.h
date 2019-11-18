#ifndef ISOPOPETRAPEZOID_H
#define ISOPOPETRAPEZOID_H

#include "trapezoid.h"

class IsopopeTrapezoid : public Trapezoid
{
    Q_OBJECT
public:
    Q_INVOKABLE IsopopeTrapezoid(Resource * res);

    IsopopeTrapezoid(QPointF const & pt);

    IsopopeTrapezoid(IsopopeTrapezoid const & o);

public:
    virtual QPointF point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // ISOPOPETRAPEZOID_H
