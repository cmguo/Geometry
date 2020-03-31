#ifndef REGULARPOLYGON_H
#define REGULARPOLYGON_H

#include "polygon.h"

class RegularPolygon : public Polygon
{
    Q_OBJECT

    Q_PROPERTY(int edges READ edges WRITE setEdges)
    Q_PROPERTY(int span READ span WRITE setSpan)
    Q_PROPERTY(int edgeSpan READ edgeSpan WRITE setEdges)

public:
    Q_INVOKABLE RegularPolygon(Resource * res);

    RegularPolygon(QPointF const & pt);

    Q_INVOKABLE RegularPolygon(RegularPolygon const & o);

public:
    virtual int pointCount() override;

    virtual QPointF iterPoint(int index, QPointF & hint) override;

    virtual QPointF nextPoint(int index, QPointF & hint) override;

    virtual QPointF prevPoint(int index, QPointF & hint) override;

    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;

public slots:
    void setEdges(int n);

    int edges() { return nEdges_; }

    void setSpan(int n);

    int span() { return nSpan_; }

    int edgeSpan();

private:
    int nEdges_;
    int nSpan_;
    QPointF vAngleStep_;
    QPointF vAngleAttach_;
};

#endif // REGULARPOLYGON_H
