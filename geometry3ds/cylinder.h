#ifndef CYLINDER_H
#define CYLINDER_H

#include "frustumcone.h"

class Cylinder : public FrustumCone
{
    Q_OBJECT
public:
    Q_INVOKABLE Cylinder(Resource * res);

    Cylinder(QPointF const & pt);

    Cylinder(Cylinder const & o);

protected:
    virtual qreal r2(qreal r) override;
};

#endif // CYLINDER_H
