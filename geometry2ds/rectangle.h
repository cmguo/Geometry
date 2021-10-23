#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "polygon.h"

class Rectangle : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE Rectangle(Resource * res);

    Q_INVOKABLE Rectangle(Rectangle const & o);

    Rectangle(QPointF const & vec1, QPointF const & vec2);

public:
    virtual int pointCount() override;

    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual QPointF point(int index) override;

    virtual bool moveElememt(int elem, const QPointF &pt) override;
};

#endif // RECTANGLE_H
