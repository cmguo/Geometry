#ifndef ISOSCELESTRIANGLE_H
#define ISOSCELESTRIANGLE_H

#include "triangle.h"

class IsoscelesTriangle : public Triangle
{
    Q_OBJECT
public:
    Q_INVOKABLE IsoscelesTriangle(Resource * res);

    IsoscelesTriangle(QPointF const & pt);

    Q_INVOKABLE IsoscelesTriangle(IsoscelesTriangle const & o);

public:
    virtual QPointF point(int index) override;

    virtual bool move(int elem, QPointF const & pt) override;
};
#endif // ISOSCELESTRIANGLE_H
