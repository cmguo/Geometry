#include "geometryplugin.h"
#include "exports.h"

GeometryPlugin::GeometryPlugin(QObject *parent)
    : QGenericPlugin(parent)
{
}

QObject *GeometryPlugin::create(const QString &, const QString &)
{
    return nullptr;
}
