#ifndef REGULARPRISM_H
#define REGULARPRISM_H

#include "polyhedron.h"

class RegularPrism : public Polyhedron
{
    Q_OBJECT
    Q_PROPERTY(int edges READ edges WRITE setEdges)

public:
    Q_INVOKABLE RegularPrism(Resource * res);

    RegularPrism(QPointF const & pt);

    Q_INVOKABLE RegularPrism(RegularPrism const & o);

public slots:
    void setEdges(int n);

    int edges() { return nEdges_; }

public:
    virtual int pointCount() override;

    virtual QVector3D point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;

    virtual QVector<QPointF> movePoints() override;

protected:
    int nEdges_;
    QVector3D origin_;
    QVector3D xyh_;
    QPointF vAngleStep_;
    QPointF vAngleInit_;
};

#endif // REGULARPRISM_H
