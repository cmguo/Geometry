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

#ifndef QT_DEBUG

static char const * const geometry2ds[] = {
    "line", "dotline", "line.so-", "line.so-so",
    "circle", "isoscelestriangle",  "righttriangle",
    "square", "isopopetrapezoid", "righttrapezoid", "regularpolygon.5", "parallelogram",
    "arbitrarypolygon", "sector", "diamond", "rectangle"
};

static char const * const geometry3ds[] = {
    "cube", "cuboid", "cylinder", "cone", "sphere",
};

#endif

void GeometryTool::getToolButtons(QList<ToolButton*> & result, QByteArray const & type)
{
    GeometryHelper::init();
    QList<QByteArray> names;
    ResourceFactory * factory = ResourceManager::instance()->getFactory(type);
#ifdef QT_DEBUG
    names = factory->resourceTypes();
#else
    if (type == "geometry2d") {
        for (auto g : geometry2ds)
            names.append(g);
    } else if (type == "geometry3d") {
        for (auto g : geometry3ds)
            names.append(g);
    }
#endif
    ToolButton::Flags flags = {ToolButton::Dynamic};
    for (QByteArray & f : names) {
        ToolButton * btn = new ToolButton(
            {factory->newUrl(f).toString().toUtf8(), f, flags, ":geometry/icon/" + type + "/" + f + ".svg"});
        result.append(btn);
    }
}

void GeometryTool::handleToolButton(ToolButton* button, QByteArray const &)
{
     WhiteCanvas * canvas = WhiteCanvasWidget::mainInstance()->canvas();
     Control * drawControl = canvas->getToolControl("drawing");
     QObject::connect(drawControl, SIGNAL(drawFinished(bool)),
                      button, SIGNAL(delayActive(bool)));
     drawControl->setOption("newUrl", button->name());
     canvas->showToolControl(drawControl);
}

GeometryTool::GeometryTool(ResourceView *res)
    : MenuTool(res)
{
}

void GeometryTool::getToolButtons(QList<ToolButton *> &buttons, ToolButton *parent)
{
    if (parent)
        return;
    QByteArray type = res_->resource()->type();
    type.remove(type.length() - 4, 4); // "tool"
    getToolButtons(buttons, type);
}

bool GeometryTool::handleToolButton(ToolButton * button, QStringList const &)
{
    WhiteCanvas * canvas = static_cast<WhiteCanvas*>(item_->parentItem()->parentItem());
    canvas->getToolControl("drawing")->setOption("newUrl", button->name());
    canvas->showToolControl("drawing");
    return true;
}
