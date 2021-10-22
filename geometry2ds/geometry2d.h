#ifndef GEOMETRY2D_H
#define GEOMETRY2D_H

#include "base/geometry.h"

#include <core/resourcefactory.h>

#include <QPainterPath>

class Geometry2D : public Geometry
{
    Q_OBJECT
protected:
    Geometry2D(Resource * res, Flags flags = {}, Flags clearFlags = {});

    Geometry2D(Geometry2D const & o);

    Geometry2D(QVector<QPointF> const & points);

public:
    virtual bool empty() const override;

protected:
    static void moveLine(QPointF const & llpt, QPointF & lpt, QPointF const & pt,
                    QPointF & npt, QPointF const & nnpt);

    static void addAngleLabeling(QPainterPath & graphPath, QPainterPath &textPath, QPointF const & lpt, QPointF const & pt,
                          QPointF const & npt);

    static void addAngleLabeling(QPainterPath & graphPath, QPainterPath &textPath, QPointF const & lpt, QPointF const & pt,
                          QPointF const & npt, qreal angle);
};

class Geometry2DFactory : ResourceFactory
{
    Q_OBJECT
public:
    Q_INVOKABLE Geometry2DFactory();

public:
    virtual ResourceView * create(Resource * res) override;

    virtual QUrl newUrl(const QByteArray &type) const override;
};

#define REGISTER_GEOMETRY_2D(ctype, type) \
    REGISTER_RESOURCE_VIEW_WITH_FACTORY(Geometry2D, ctype, type)

#endif // GEOMETRY2D_H
