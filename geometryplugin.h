#ifndef GEOMETRYPLUGIN_H
#define GEOMETRYPLUGIN_H

#include <qcomponentfactoryinterface.h>

#include <QGenericPlugin>

class GeometryPlugin : public QGenericPlugin
        , public QComponentFactoryInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ComponentFactory_iid FILE "Geometry.json")
    Q_INTERFACES(QComponentFactoryInterface)

public:
    explicit GeometryPlugin(QObject *parent = nullptr);

private:
    QObject *create(const QString &name, const QString &spec) override;
};

#endif // GEOMETRYPLUGIN_H
