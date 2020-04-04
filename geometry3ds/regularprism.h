#ifndef REGULARPRISM_H
#define REGULARPRISM_H

#include "regularprismoid.h"

class RegularPrism : public RegularPrismoid
{
    Q_OBJECT
public:
    Q_INVOKABLE RegularPrism(Resource * res);

    Q_INVOKABLE RegularPrism(RegularPrism const & o);
};

#endif // REGULARPRISM_H
