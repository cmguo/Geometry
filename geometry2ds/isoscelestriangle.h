#ifndef ISOSCELESTRIANGLE_H
#define ISOSCELESTRIANGLE_H

#include "triangle.h"

class IsoscelesTriangle : public Triangle
{
    Q_OBJECT
public:
    Q_INVOKABLE IsoscelesTriangle(Resource * res);

    Q_INVOKABLE IsoscelesTriangle(IsoscelesTriangle const & o);

    IsoscelesTriangle(QPointF const & vec1, QPointF const & vec2);

public:
    virtual QPointF point(int index) override;

    virtual bool moveElememt(int elem, QPointF const & pt) override;
};
#endif // ISOSCELESTRIANGLE_H
