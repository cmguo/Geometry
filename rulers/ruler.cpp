#include "ruler.h"

#include <core/resource.h>

RulerFactory::RulerFactory()
{
}

ResourceView *RulerFactory::create(Resource *res)
{
    QString type = res->url().path();
    int n = type.indexOf('/');
    if (n > 0)
        type = type.left(n);
    return ResourceFactory::create(res, type.toUtf8());
}

QUrl RulerFactory::newUrl(const QByteArray &type) const
{
    return QUrl("ruler:" + type);
}
