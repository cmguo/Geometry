#ifndef ARBITRARYPOLYGON_H
#define ARBITRARYPOLYGON_H

#include "polygon.h"

class ArbitraryPolygon : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE ArbitraryPolygon(Resource * res);

    ArbitraryPolygon(QPointF const & pt);

    ArbitraryPolygon(ArbitraryPolygon const & o);

public:
    virtual bool commitPoint(QPointF const & pt) override;

    virtual bool moveTempPoint(const QPointF &pt) override;

    virtual bool canFinish() override;

    virtual int pointCount() override;

    virtual QPointF point(int index) override;

private:
    bool tempValid_ = false;
    QPointF temp_;
};

#endif // ARBITRARYPOLYGON_H
