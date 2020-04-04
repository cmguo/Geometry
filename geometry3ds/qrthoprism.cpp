#include "qrthoprism.h"

static EdgesToolButtons edgesButtons("正%1棱锥");
REGISTER_OPTION_BUTTONS(Qrthoprism, edges, edgesButtons)

Qrthoprism::Qrthoprism(Resource * res)
    : RegularPrismoid(res, 0)
{
}

Qrthoprism::Qrthoprism(Qrthoprism const & o)
    : RegularPrismoid(o)
{
}
