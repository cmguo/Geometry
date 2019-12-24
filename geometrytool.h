#ifndef GEOMETRYTOOL_H
#define GEOMETRYTOOL_H

#include "Geometry_global.h"

#include <core/widgetcontrol.h>

class GEOMETRY_EXPORT GeometryTool : public WidgetControl
{
    Q_OBJECT

public:
    static void getToolButtons(QList<ToolButton*> & result, QString const & type);

    static void handleToolButton(ToolButton* button);

public:
    Q_INVOKABLE GeometryTool(ResourceView *res);

private:
    virtual QWidget * createWidget(ResourceView *res) override;

    virtual void attached() override;

private:
    void buttonClicked(QList<ToolButton *> const & buttons);

private:
    QList<ToolButton *> buttons_;
};

#endif // GRAPHTOOL_H
