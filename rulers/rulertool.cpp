#include "rulertool.h"
#include "triangleruleritem.h"
#include "protractoritem.h"
#include "linearruleritem.h"

#include <core/resourceview.h>
#include <QUrl>

#include <cmath>

RulerTool::RulerTool(ResourceView * res)
    : Control(res, {KeepAspectRatio})
{
}

ControlView * RulerTool::create(ControlView * parent)
{
    (void) parent;
    QString path = res_->url().path();
    QString type = res_->url().path().split("/")[0];
    QGraphicsItem *item = nullptr;
    if (type == "linear") {
        item = new LinearRulerItem;
    }else if(type == "triangle") {
        item = new TriangleRulerItem(false);
    }else if(type == "protractor") {
        item = new ProtractorItem;
    } else if(type == "iso_triangle") {
        item = new TriangleRulerItem(true);
    }
    return item;
}

void RulerTool::attaching()
{

}

void RulerTool::attached()
{
    loadFinished(true);
}

QString RulerTool::toolsString(QByteArray const &) const
{
    return nullptr;
}
