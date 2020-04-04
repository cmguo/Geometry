#ifndef ARBITRARYPOLYGON_H
#define ARBITRARYPOLYGON_H

#include "polygon.h"

class ArbitraryPolygon : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE ArbitraryPolygon(Resource * res);

    Q_INVOKABLE ArbitraryPolygon(ArbitraryPolygon const & o);

public:
    virtual void addPoint(QPointF const & pt) override;

    virtual void movePoint(QPointF const & pt) override;

    virtual bool commitPoint(QPointF const & pt) override;

    virtual bool moveTempPoint(const QPointF &pt) override;

    virtual bool canFinish() override;

    virtual void finish(const QPointF &c) override;

    virtual int pointCount() override;

    virtual QPointF point(int index) override;

private:
    bool tempValid_ = false;
    QPointF temp_;
};

#endif // ARBITRARYPOLYGON_H
