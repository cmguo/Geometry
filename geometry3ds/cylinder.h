#ifndef CYLINDER_H
#define CYLINDER_H

#include "frustumcone.h"

class Cylinder : public FrustumCone
{
    Q_OBJECT
public:
    Q_INVOKABLE Cylinder(Resource * res);

    Q_INVOKABLE Cylinder(Cylinder const & o);

public:
    virtual bool moveElememt(int elem, QPointF const & pt) override;

protected:
    virtual qreal r2(qreal r) override;
};

#endif // CYLINDER_H
