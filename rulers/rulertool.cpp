#include "rulertool.h"
#include "ruleritem.h"
#include "ruler.h"

#include <core/resourceview.h>

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

QString RulerTool::toolsString(QByteArray const &) const
{
    return nullptr;
}
