#include "geometry3d.h"

#include <core/resource.h>

#include <QtMath>

/*
 *              Z ^      _/ Y
 *                |    _/
 *                |  _/
 *                | /
 *                o---------------> X
 */
QMatrix4x4 Geometry3D::PO(
        1, float(CO), 0, 0,
        0, -float(CO), -1, 0,
        0, 0, 0, 0,
        0, 0, 0, 1);

/*
 *              Z ^
 *                |
 *                |
 *                |
 *                o
 *              /   \
 *           /         \
 *        /               \
 *   X /                     \ Y
 */
QMatrix4x4 Geometry3D::PI(
        -float(CIX), float(CIX), 0, 0,
        float(CIY), float(CIY), -float(CIY) * 2, 0,
        0, 0, 0, 0,
        0, 0, 0, 1);

Geometry3D::Geometry3D(Resource * res)
    : Geometry(res)
{
}

Geometry3D::Geometry3D(Geometry3D const & o)
    : Geometry(o)
    , origin_(o.origin_)
    , size_(o.size_)
{
}

bool Geometry3D::empty() const
{
    return Geometry::empty() && res_->url().path().lastIndexOf('/') == 0
            && res_->url().query().length() == 0;
}

qreal Geometry3D::length(QVector3D const & vec)
{
    (void) vec;
    return 0;//sqrt(QPointF::dotProduct(vec, vec));
}


Geometry3DFactory::Geometry3DFactory()
{
}

ResourceView * Geometry3DFactory::create(Resource *res)
{
    QString type = res->url().path().mid(1);
    int n = type.indexOf('/');
    if (n > 0)
        type = type.left(n);
    return ResourceFactory::create(res, type.toUtf8());
}

QUrl Geometry3DFactory::newUrl(const QByteArray &type) const
{
    return QUrl("geometry3d:///" + type);
}
