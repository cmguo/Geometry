#ifndef SPHERE_H
#define SPHERE_H

#include "ellipsoid.h"

class Sphere : public Ellipsoid
{
    Q_OBJECT
public:
    Q_INVOKABLE Sphere(Resource * res);

    Q_INVOKABLE Sphere(Sphere const & o);

public:
    virtual void sync() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF & pt) override;

    virtual bool move(int elem, QPointF const & pt) override;

};

#endif // SPHERE_H
