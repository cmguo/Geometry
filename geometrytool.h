#ifndef GEOMETRYTOOL_H
#define GEOMETRYTOOL_H

#include "Geometry_global.h"

#include <core/widgetcontrol.h>

class GEOMETRY_EXPORT GeometryTool : public WidgetControl
{
    Q_OBJECT
public:
    Q_INVOKABLE GeometryTool(ResourceView *res);

private:
    virtual QWidget * createWidget(ResourceView *res) override;

private:
    void buttonClicked(QList<ToolButton *> const & buttons);

private:
    QList<ToolButton *> buttons_;
};

#endif // GRAPHTOOL_H
