#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "polygon.h"

class Triangle : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE Triangle(Resource * res);

    Triangle(Triangle const & o);

public:
    virtual int pointCount() override;

    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;

private:
    qreal angles_[3];
};

#endif // TRIANGLE_H
