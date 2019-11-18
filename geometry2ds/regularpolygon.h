#ifndef REGULARPOLYGON_H
#define REGULARPOLYGON_H

#include "polygon.h"

class RegularPolygon : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE RegularPolygon(Resource * res);

    RegularPolygon(QPointF const & pt);

    RegularPolygon(RegularPolygon const & o);

public:
    virtual int pointCount() override;

    virtual QPointF iterPoint(int index, QPointF & hint) override;

    virtual QPointF nextPoint(int index, QPointF & hint) override;

    virtual QPointF prevPoint(int index, QPointF & hint) override;

    virtual bool setPoint(int index, const QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;

private:
    int nEdges_;
    int nSpan_;
    QPointF vAngle_;
    bool commit_ = false;
};

#endif // REGULARPOLYGON_H
