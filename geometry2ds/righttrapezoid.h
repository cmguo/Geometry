#ifndef RIGHTTRAPEZOID_H
#define RIGHTTRAPEZOID_H

#include "trapezoid.h"

class RightTrapezoid : public Trapezoid
{
    Q_OBJECT
public:
    Q_INVOKABLE RightTrapezoid(Resource * res);

    Q_INVOKABLE RightTrapezoid(RightTrapezoid const & o);

    RightTrapezoid(QPointF const & vec1, QPointF const & vec2, QPointF const & vec3);

protected:
    virtual QPointF point4(QPointF const & pt1, QPointF const & pt2, QPointF const & pt3) override;
};

#endif // RIGHTTRAPEZOID_H
