#ifndef CUBE_H
#define CUBE_H

#include "cuboid.h"

class Cube : public Cuboid
{
    Q_OBJECT
public:
    Q_INVOKABLE Cube(Resource * res);

    Q_INVOKABLE Cube(Cube const & o);

public:
    virtual void movePoint(const QPointF &pt) override;

    virtual bool moveElememt(int elem, const QPointF &pt) override;
};

#endif // CUBE_H
