#ifndef RIGHTTRIANGLE_H
#define RIGHTTRIANGLE_H

#include "triangle.h"

class RightTriangle : public Triangle
{
    Q_OBJECT
public:
    Q_INVOKABLE RightTriangle(Resource * res);

    Q_INVOKABLE RightTriangle(RightTriangle const & o);

public:
    virtual QPointF point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // RIGHTTRIANGLE_H
