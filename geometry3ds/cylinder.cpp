#include "cylinder.h"

Cylinder::Cylinder(Resource * res)
    : FrustumCone(res)
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

bool Cylinder::moveElememt(int elem, QPointF const & pt)
{
    if (elem == 4 || elem == 5) {
        points_[0].setY(pt.y());
        QPointF p(pt.x(), points_[1].y());
        return FrustumCone::moveElememt(1, p);
    }
    return FrustumCone::moveElememt(elem, pt);
}
