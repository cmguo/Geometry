#ifndef GEOMETRYTOOL_H
#define GEOMETRYTOOL_H

#include "Geometry_global.h"

#include <tools/menutool.h>

class GEOMETRY_EXPORT GeometryTool : public MenuTool
{
    Q_OBJECT

public:
    static void getToolButtons(QList<ToolButton*> & result, QString const & type);

    static void handleToolButton(ToolButton* button, QString const & type);

public:
    Q_INVOKABLE GeometryTool(ResourceView *res);

private:
    virtual void getToolButtons(QList<ToolButton *> & buttons,
                                ToolButton * parent) override;

    virtual void handleToolButton(ToolButton * button) override;

private:
    QList<ToolButton *> buttons_;
};

#endif // GRAPHTOOL_H
