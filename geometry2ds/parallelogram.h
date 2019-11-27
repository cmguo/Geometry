#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include "trapezoid.h"

class Parallelogram : public Trapezoid
{
    Q_OBJECT
public:
    Q_INVOKABLE Parallelogram(Resource * res);

    Parallelogram(QPointF const & pt);

    Q_INVOKABLE Parallelogram(Parallelogram const & o);

public:
    virtual QPointF point(int index) override;

    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // PARALLELOGRAM_H
