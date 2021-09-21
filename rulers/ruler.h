#ifndef RULER_H
#define RULER_H

#include <core/resourcefactory.h>

class RulerFactory : ResourceFactory
{
    Q_OBJECT
public:
    Q_INVOKABLE RulerFactory();

public:
    virtual ResourceView * create(Resource * res) override;

    virtual QUrl newUrl(const QByteArray &type) const override;
};

#define REGISTER_GEOMETRY_2D(ctype, type) \
    REGISTER_RESOURCE_VIEW_WITH_FACTORY(Geometry2D, ctype, type)

#endif // RULER_H
