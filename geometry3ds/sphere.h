#ifndef SPHERE_H
#define SPHERE_H

#include "geometry3d.h"

class Sphere : public Geometry3D
{
    Q_OBJECT
public:
    Q_INVOKABLE Sphere(Resource * res);

    Sphere(QPointF const & pt);

    Sphere(Sphere const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF & pt) override;

    virtual bool move(int elem, QPointF const & pt) override;

public slots:
    void draw(QPainter *painter);
};

#endif // SPHERE_H
