#include "cylinder.h"

Cylinder::Cylinder(Resource * res)
    : FrustumCone(res)
{
}

Cylinder::Cylinder(QPointF const & pt)
    : FrustumCone(pt)
{
}

Cylinder::Cylinder(Cylinder const & o)
    : FrustumCone(o)
{
}

qreal Cylinder::r2(qreal r)
{
    return r;
}
