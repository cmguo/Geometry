#ifndef FRUSTUMCONE_H
#define FRUSTUMCONE_H

#include "geometry3d.h"

class FrustumCone : public Geometry3D
{
    Q_OBJECT
public:
    Q_INVOKABLE FrustumCone(Resource * res);

    FrustumCone(QPointF const & pt);

    FrustumCone(FrustumCone const & o);

public:
    virtual QPainterPath path() override;

public slots:
    void draw(QPainter *painter);

protected:
    virtual qreal r2(qreal r);
};

#endif // FRUSTUMCONE_H
