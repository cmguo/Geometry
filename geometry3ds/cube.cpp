#include "cube.h"

Cube::Cube(Resource * res)
    : Cuboid(res)
{
}

Cube::Cube(QPointF const & pt)
    : Cuboid(pt)
{
}

Cube::Cube(Cube const & o)
    : Cuboid(o)
{
}

