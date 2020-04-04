#ifndef QRTHOPRISM_H
#define QRTHOPRISM_H

#include "regularprismoid.h"

class Qrthoprism : public RegularPrismoid
{
    Q_OBJECT
public:
    Q_INVOKABLE Qrthoprism(Resource * res);

    Q_INVOKABLE Qrthoprism(Qrthoprism const & o);
};

#endif // QRTHOPRISM_H
