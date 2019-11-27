#ifndef CONE_H
#define CONE_H

#include "frustumcone.h"

class Cone : public FrustumCone
{
    Q_OBJECT
public:
    Q_INVOKABLE Cone(Resource * res);

    Cone(QPointF const & pt);

    Q_INVOKABLE Cone(Cone const & o);

protected:
    virtual qreal r2(qreal r) override;
};

#endif // CONE_H
