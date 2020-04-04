#ifndef REGULARPRISM_H
#define REGULARPRISM_H

#include "regularprismoid.h"

class RegularPrism : public RegularPrismoid
{
    Q_OBJECT
public:
    Q_INVOKABLE RegularPrism(Resource * res);

    RegularPrism(QPointF const & pt);

    Q_INVOKABLE RegularPrism(RegularPrism const & o);

protected:
    virtual qreal r2(qreal r) override;
};

#endif // REGULARPRISM_H
