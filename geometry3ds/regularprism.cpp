#include "regularprism.h"

RegularPrism::RegularPrism(Resource * res)
    : RegularPrismoid(res)
{
}

RegularPrism::RegularPrism(QPointF const & pt)
    : RegularPrismoid(pt)
{
}

RegularPrism::RegularPrism(RegularPrism const & o)
    : RegularPrismoid(o)
{
}

qreal RegularPrism::r2(qreal r)
{
    return r;
}
