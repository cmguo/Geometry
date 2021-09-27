#include "geometrytool.h"
#include "base/geometryhelper.h"

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
    "line", "dotline", "line.so-", "line.so-so", "", "",
    "arbitrarypolygon", "circle", "sector", "isoscelestriangle", "righttriangle", "parallelogram", "diamond",
    "square", "rectangle", "isopopetrapezoid", "righttrapezoid", "regularpolygon.5",
};

static char const * const geometry3ds[] = {
    "cube", "cuboid", "cylinder", "cone", "sphere",
};

static char const * const rulertools[] = {
    "linear", "triangle", "iso_triangle", "protractor", "compasses",
};

#endif

GeometryTools::GeometryTools()
{
}

void GeometryTools::getToolButtons(QList<ToolButton*> & result, QByteArray const & type)
{
    GeometryHelper::init();
    static QMap<QByteArray, QList<ToolButton*>> gButtons;
    if (gButtons.contains(type)) {
        result = gButtons.value(type);
        return;
    }
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
        ToolButton * btn = f.isEmpty() ? &ToolButton::PLACE_HOOLDER : new ToolButton(
            {factory->newUrl(f).toString().toUtf8(), f, flags, ":geometry/icon/" + type + "/" + f + ".svg"});
        result.append(btn);
    }
    gButtons.insert(type, result);
}

void GeometryTools::handleToolButton(ToolButton* button, QByteArray const &)
{
    if (button == &ToolButton::PLACE_HOOLDER)
        return;
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
    GeometryTools::getToolButtons(buttons, type);
}

bool GeometryTool::handleToolButton(ToolButton * button, QStringList const &)
{
    WhiteCanvas * canvas = static_cast<WhiteCanvas*>(item_->parentItem()->parentItem());
    if (button->name().startsWith("rulertool:")) {
        Control * control = canvas->addResource(QUrl(button->name()));
        control->item()->show();
        return true;
    }
    canvas->getToolControl("drawing")->setOption("newUrl", button->name());
    canvas->showToolControl("drawing");
    return true;
}

