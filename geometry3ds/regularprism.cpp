#include "regularprism.h"

static EdgesToolButtons edgesButtons("正%1棱柱");
REGISTER_OPTION_BUTTONS(RegularPrism, edges, edgesButtons)

RegularPrism::RegularPrism(Resource * res)
    : RegularPrismoid(res, 1.0)
{
    ratio_ = 1.0;
}

RegularPrism::RegularPrism(RegularPrism const & o)
    : RegularPrismoid(o)
{
}
