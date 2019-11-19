#ifndef GEOMETRY2D_H
#define GEOMETRY2D_H

#include "geometry.h"

#include <core/resourcefactory.h>

#include <QPainterPath>

class Geometry2D : public Geometry
{
    Q_OBJECT
public:
    Geometry2D(Resource * res);

    Geometry2D(QPointF const & pt);

    Geometry2D(Geometry2D const & o);

public:
    virtual bool empty() const override;

public:
    static qreal angle(QPointF const & vec);

    static void rotate(QPointF & pt, QPointF const & angle);

    static qreal crossProduct(QPointF const & p1, QPointF const & p2);

    static qreal dist2PointToSegment(QPointF const & p1, QPointF const & p2,
                                    QPointF const & p, QPointF & rp);

    static QPointF nearestPointAtVerticalBisector(QPointF const & p1, QPointF const & p2,
                                   QPointF const & p);

    static void adjustToLength(QPointF const & start, QPointF & end, qreal length);
};

class Geometry2DFactory : ResourceFactory
{
    Q_OBJECT
public:
    Q_INVOKABLE Geometry2DFactory();

public:
    virtual ResourceView * create(Resource * res) override;

    virtual QUrl newUrl(const QString &type) const override;
};

#define REGISTER_GEOMETRY_2D(ctype, type) \
    REGISTER_RESOURCE_VIEW_WITH_FACTORY(Geometry2D, ctype, type)

#endif // GEOMETRY2D_H
