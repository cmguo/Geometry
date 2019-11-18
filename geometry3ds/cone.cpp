#include "cone.h"

Cone::Cone(Resource * res)
    : FrustumCone(res)
{
}

Cone::Cone(QPointF const & pt)
    : FrustumCone(pt)
{
}

Cone::Cone(Cone const & o)
    : FrustumCone(o)
{
}

qreal Cone::r2(qreal r)
{
    (void)r;
    return 0;
}
