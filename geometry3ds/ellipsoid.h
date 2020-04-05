#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include "geometry3d.h"

class Ellipsoid : public Geometry3D
{
    Q_OBJECT
public:
    Q_INVOKABLE Ellipsoid(Resource * res);

    Q_INVOKABLE Ellipsoid(Ellipsoid const & o);

public:
    virtual void sync() override;

    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF & pt) override;

    virtual bool move(int elem, QPointF const & pt) override;

public slots:
    void draw(QPainter *painter);

protected:
    QVector3D radius_;
};

#endif // ELLIPSOID_H
