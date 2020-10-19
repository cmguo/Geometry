#include "rulergaugecontrol.h"
#include "triangleruler.h"
#include "protractor.h"
#include "lineruler.h"

#include <core/resourceview.h>
#include <QUrl>

#include <cmath>

RulerGaugeControl::RulerGaugeControl(ResourceView * res)
    : Control(res, {KeepAspectRatio})
{
}

ControlView * RulerGaugeControl::create(ControlView * parent)
{
    QString path = res_->url().path();
    QString type = res_->url().path().split("/")[1];
    QGraphicsItem *item = nullptr;
    if(type == "ruler"){
        item =new Lineruler(600,150,nullptr);
    }else if(type == "triangler"){
        item =new TriangleRuler(false,500,500*sqrt(3)/3,nullptr);
    }else if(type == "protractor"){
        item =new Protractor(500,250,nullptr);
    } else if(type == "iso_triangler"){
        item =new TriangleRuler(true,500,500,nullptr);
    }

    return item;
}

void RulerGaugeControl::attaching()
{

}

void RulerGaugeControl::attached()
{
    loadFinished(true);
}

QString RulerGaugeControl::toolsString(QByteArray const &) const
{
    return nullptr;
}
