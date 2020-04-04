#ifndef CUBOID_H
#define CUBOID_H

#include "polyhedron.h"

class Cuboid : public Polyhedron
{
    Q_OBJECT
public:
    Q_INVOKABLE Cuboid(Resource * res);

    Q_INVOKABLE Cuboid(Cuboid const & o);

public:
    virtual int pointCount() override;

    virtual QVector3D point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;

protected:
    void setMoveElem(int elem);

protected:
    int moveElem_ = 1;
    bool inner_ = false; // inner control point 2,3,5,6
};

#endif // CUBOID_H
