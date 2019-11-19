#include "geometrytool.h"
#include "exports.h"

#include <core/resourcefactory.h>
#include <core/resourcemanager.h>
#include <core/toolbutton.h>
#include <core/resource.h>
#include <views/toolbarwidget.h>
#include <views/whitecanvas.h>

#include <QVBoxLayout>

GeometryTool::GeometryTool(ResourceView *res)
    : WidgetControl(res, FullLayout, CanSelect)
{
    QString type = res_->resource()->type();
    type.remove("tool");
    ResourceFactory * factory = ResourceManager::instance()->getFactory(type);
    for (QString const & f : factory->resourceTypes()) {
        ToolButton * btn = new ToolButton(
            {factory->newUrl(f).toString(), f, nullptr, QVariant()});
        buttons_.append(btn);
    }
}

QWidget * GeometryTool::createWidget(ResourceView *res)
{
    (void) res;
    ToolbarWidget * widget = new ToolbarWidget(false);
    widget->setToolButtons(buttons_);
    void (ToolbarWidget::*sig)(QList<ToolButton *> const &) = &ToolbarWidget::buttonClicked;
    QObject::connect(widget, sig, this, &GeometryTool::buttonClicked);
    return widget;
}

void GeometryTool::buttonClicked(QList<ToolButton *> const & buttons)
{
    WhiteCanvas * canvas = static_cast<WhiteCanvas*>(item_->parentItem()->parentItem());
    canvas->getToolControl("drawing")->setProperty("newUrl", buttons.back()->name);
    canvas->showToolControl("drawing");
}
