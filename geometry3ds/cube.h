#ifndef CUBE_H
#define CUBE_H

#include "cuboid.h"

class Cube : public Cuboid
{
    Q_OBJECT
public:
    Q_INVOKABLE Cube(Resource * res);

    Cube(QPointF const & pt);

    Cube(Cube const & o);

public:
    virtual void movePoint(const QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // CUBE_H
