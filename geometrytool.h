#ifndef GEOMETRYTOOL_H
#define GEOMETRYTOOL_H

#include "Geometry_global.h"

#include <tools/menutool.h>

class GEOMETRY_EXPORT GeometryTool : public MenuTool
{
    Q_OBJECT

public:
    static void getToolButtons(QList<ToolButton*> & result, QByteArray const & type);

    static void handleToolButton(ToolButton* button, QByteArray const & type);

public:
    Q_INVOKABLE GeometryTool(ResourceView *res);

private:
    virtual void getToolButtons(QList<ToolButton *> & buttons,
                                ToolButton * parent) override;

    virtual void handleToolButton(ToolButton * button, QStringList const & args) override;

private:
    QList<ToolButton *> buttons_;
};

#endif // GRAPHTOOL_H
