#include "rulertool.h"
#include "ruleritem.h"
#include "ruler.h"
#include "base/geometry.h"

#include <core/resourceview.h>
#include <views/whitecanvas.h>
#include <views/itemselector.h>

#include <QUrl>

#include <cmath>

RulerTool::RulerTool(ResourceView * res)
    : Control(res, {KeepAspectRatio}, {CanSelect, CanRotate, CanScale})
{
}

ControlView * RulerTool::create(ControlView * parent)
{
    (void) parent;
    return new RulerItem(qobject_cast<Ruler*>(res_));
}

void RulerTool::attaching()
{
    if (!flags_.testFlag(RestoreSession)) {
        qobject_cast<Ruler*>(res_)->updateShape();
    }
}

void RulerTool::attached()
{
    loadFinished(true);
}

Control::SelectMode RulerTool::selectTest(ControlView *child, ControlView *, const QPointF &point, bool)
{
    return (child != item_ || qobject_cast<Ruler*>(res_)->hitTestTickMark(point)) ? SelectMode::NotSelect : SelectMode::Select;
}

QString RulerTool::toolsString(QByteArray const &) const
{
    return nullptr;
}

Control * RulerTool::addGeometry(Geometry *geometry)
{
    return whiteCanvas()->addResource(geometry);
}

void RulerTool::finishGeometry(Control *geometry)
{
    whiteCanvas()->selector()->unselect(geometry);
}
