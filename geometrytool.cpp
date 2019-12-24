#include "geometrytool.h"
#include "exports.h"

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
    "arbitrarypolygon","sector","diamond","rectangle"
};

static char const * const geometry3ds[] = {
    "cube", "cuboid", "cylinder", "cone", "sphere"
};

void GeometryTool::getToolButtons(QList<ToolButton*> & result, QString const & type)
{
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
            {factory->newUrl(f).toString(), f, flags, ":geometry/icons/" + type + "/" + f + ".svg"});
        result.append(btn);
    }
}

void GeometryTool::handleToolButton(ToolButton* button)
{
     WhiteCanvas * canvas = WhiteCanvasWidget::mainInstance()->canvas();
     canvas->getToolControl("drawing")->setProperty("newUrl", button->name);
     canvas->showToolControl("drawing");
}

GeometryTool::GeometryTool(ResourceView *res)
    : WidgetControl(res, {PositionAtCenter}, {CanSelect, CanRotate, CanScale})
{
    QString type = res_->resource()->type();
    type.remove("tool");
    getToolButtons(buttons_, type);
}

QWidget * GeometryTool::createWidget(ResourceView *res)
{
    (void) res;
    ToolbarWidget * widget = new ToolbarWidget(false);
    return widget;
}

void GeometryTool::attached()
{
    ToolbarWidget * widget = static_cast<ToolbarWidget*>(widget_);
    widget->setToolButtons(buttons_);
    void (ToolbarWidget::*sig)(QList<ToolButton *> const &) = &ToolbarWidget::buttonClicked;
    QObject::connect(widget, sig, this, &GeometryTool::buttonClicked);
    loadFinished(true);
}

void GeometryTool::buttonClicked(QList<ToolButton *> const & buttons)
{
    WhiteCanvas * canvas = static_cast<WhiteCanvas*>(item_->parentItem()->parentItem());
    canvas->getToolControl("drawing")->setProperty("newUrl", buttons.back()->name);
    canvas->showToolControl("drawing");
}
