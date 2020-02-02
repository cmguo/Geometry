#include "geometrytool.h"
#include "exports.h"
#include "geometryhelper.h"

#include <core/resourcefactory.h>
#include <core/resourcemanager.h>
#include <core/toolbutton.h>
#include <core/resource.h>
#include <views/toolbarwidget.h>
#include <views/whitecanvas.h>
#include <views/whitecanvaswidget.h>

#include <QVBoxLayout>

static char const * const geometry2ds[] = {
    "line", "dotline", "line.so-", "line.so-so",
    "circle", "isoscelestriangle",  "righttriangle",
    "square", "isopopetrapezoid", "righttrapezoid", "regularpolygon.5", "parallelogram",
    "arbitrarypolygon", "sector", "diamond", "rectangle"
};

static char const * const geometry3ds[] = {
    "cube", "cuboid", "cylinder", "cone", "sphere",
    #ifdef QT_DEBUG
    "frustumcone"
    #endif
};

void GeometryTool::getToolButtons(QList<ToolButton*> & result, QString const & type)
{
    GeometryHelper::init();
    char const * const * geometries = nullptr;
    int count = 0;
    if (type == "geometry2d") {
        geometries = geometry2ds;
        count = sizeof(geometry2ds) / sizeof(geometry2ds[0]);
    } else if (type == "geometry3d") {
        geometries = geometry3ds;
        count = sizeof(geometry3ds) / sizeof(geometry3ds[0]);
    }
    ResourceFactory * factory = ResourceManager::instance()->getFactory(type);
    ToolButton::Flags flags = {ToolButton::Dynamic};
    for (int i = 0; i < count; ++i) {
        char const * f = geometries[i];
        ToolButton * btn = new ToolButton(
            {factory->newUrl(f).toString().toUtf8(), f, flags, ":geometry/icons/" + type + "/" + f + ".svg"});
        result.append(btn);
    }
}

void GeometryTool::handleToolButton(ToolButton* button, QString const &)
{
     WhiteCanvas * canvas = WhiteCanvasWidget::mainInstance()->canvas();
     canvas->getToolControl("drawing")->setOption("newUrl", button->name);
     canvas->showToolControl("drawing");
}

GeometryTool::GeometryTool(ResourceView *res)
    : MenuTool(res)
{
}

void GeometryTool::getToolButtons(QList<ToolButton *> &buttons, ToolButton *parent)
{
    if (parent)
        return;
    QString type = res_->resource()->type();
    type.remove("tool");
    getToolButtons(buttons, type);
}

void GeometryTool::handleToolButton(ToolButton * button)
{
    WhiteCanvas * canvas = static_cast<WhiteCanvas*>(item_->parentItem()->parentItem());
    canvas->getToolControl("drawing")->setOption("newUrl", button->name);
    canvas->showToolControl("drawing");
}
