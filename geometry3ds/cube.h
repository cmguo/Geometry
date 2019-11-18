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
};

#endif // CUBE_H
