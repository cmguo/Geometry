#ifndef ISOPOPETRAPEZOID_H
#define ISOPOPETRAPEZOID_H

#include "trapezoid.h"

class IsopopeTrapezoid : public Trapezoid
{
    Q_OBJECT
public:
    Q_INVOKABLE IsopopeTrapezoid(Resource * res);

    IsopopeTrapezoid(QPointF const & pt);

    Q_INVOKABLE IsopopeTrapezoid(IsopopeTrapezoid const & o);

protected:
    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual QPointF point4(QPointF const & pt1, QPointF const & pt2, QPointF const & pt3) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // ISOPOPETRAPEZOID_H
