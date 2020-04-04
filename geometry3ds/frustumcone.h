#ifndef FRUSTUMCONE_H
#define FRUSTUMCONE_H

#include "geometry3d.h"

class FrustumCone : public Geometry3D
{
    Q_OBJECT
public:
    Q_INVOKABLE FrustumCone(Resource * res);

    Q_INVOKABLE FrustumCone(FrustumCone const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF & pt) override;

    virtual bool move(int elem, QPointF const & pt) override;

public slots:
    bool contains(QPointF const & pt);

    void draw(QPainter *painter);

protected:
    virtual qreal r2(qreal r);
};

#endif // FRUSTUMCONE_H
