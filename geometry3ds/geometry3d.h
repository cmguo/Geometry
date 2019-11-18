#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H

#include "geometry.h"

#include <core/resourcefactory.h>

#include <QPainterPath>
#include <QVector3D>
#include <QMatrix4x4>

class Geometry3D : public Geometry
{
    Q_OBJECT
public:
    Geometry3D(Resource * res);

    Geometry3D(QPointF const & pt);

    Geometry3D(Geometry3D const & o);

public:
    virtual bool empty() const override;

protected:
    /* Oblique projection */

    // sqrt(2.0f) / 4.0f
    static constexpr qreal CO = 0.35355339059327376220042218105242;

    static QMatrix4x4 PO;

    /* Isometric projection */

    // sqrt(2.0f) / 2.0f
    static constexpr qreal CIX = 0.70710678118654752440084436210485;

    // sqrt(6.0f) / 6.0f
    static constexpr qreal CIY = 0.40824829046386301636621401245098;

    static QMatrix4x4 PI;

protected:
    static qreal length(QVector3D const & vec);

    static void adjustToLength(QVector3D const & start, QVector3D & end, qreal length);

protected:
    QVector3D origin_;
    QVector3D size_;
};

class Geometry3DFactory : ResourceFactory
{
    Q_OBJECT
public:
    Q_INVOKABLE Geometry3DFactory();

public:
    virtual ResourceView * create(Resource * res) override;

    virtual QUrl newUrl(const QString &type) const override;
};

#define REGISTER_GEOMETRY_3D(ctype, type) \
    REGISTER_RESOURCE_VIEW_WITH_FACTORY(Geometry3D, ctype, type)

#endif // GEOMETRY3D_H
