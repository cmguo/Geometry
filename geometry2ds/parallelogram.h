#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include "trapezoid.h"

class Parallelogram : public Trapezoid
{
    Q_OBJECT
public:
    Q_INVOKABLE Parallelogram(Resource * res);

    Q_INVOKABLE Parallelogram(Parallelogram const & o);

protected:
    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual QPointF point4(QPointF const & pt1, QPointF const & pt2, QPointF const & pt3) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // PARALLELOGRAM_H
