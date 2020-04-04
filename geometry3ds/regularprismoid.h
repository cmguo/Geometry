#ifndef REGULARPRISMOID_H
#define REGULARPRISMOID_H

#include "polyhedron.h"

class RegularPrismoid : public Polyhedron
{
    Q_OBJECT
    Q_PROPERTY(int edges READ edges WRITE setEdges)

public:
    Q_INVOKABLE RegularPrismoid(Resource * res);

    RegularPrismoid(QPointF const & pt);

    Q_INVOKABLE RegularPrismoid(RegularPrismoid const & o);

public slots:
    void setEdges(int n);

    int edges() { return nEdges_; }

public:
    virtual int pointCount() override;

    virtual QVector3D point(int index) override;

    virtual bool move(int elem, const QPointF &pt) override;

    virtual QVector<QPointF> movePoints() override;

protected:
    virtual qreal r2(qreal r);

protected:
    int nEdges_;
    qreal ratio_ = -1;
    QPointF vAngleStep_;
    QPointF vAngleInit_;
};

#endif // REGULARPRISMOID_H
