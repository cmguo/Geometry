#include "protractor.h"

#include <QPainter>

Protractor::Protractor(QGraphicsItem *parent):RulerGaugeBase(parent)
{

}

Protractor::Protractor(int width, int height, QGraphicsItem *parent):RulerGaugeBase(width,height,parent)
{
    updateShape();
    adjustControlButtonPos();
}

Protractor::~Protractor()
{

}

void Protractor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //绘制扇形背景
    QPen p = QPen(Qt::black,2);
    p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    painter->setBrush(Qt::white);
    painter->drawPath(shape_);

    p.setColor(Qt::black);
    QFont font = painter->font();
    font.setPixelSize(10);
    painter->setFont(font);
    painter->setPen(p);
    QPoint point(boundingRect().width()/2,0);
    int linelen = 10;
    for(int i=0;i<=180;i++)
    {
        painter->save();
        painter->translate(boundingRect().width()/2,boundingRect().height());
        painter->rotate(180+i);
        linelen = (i%10==0)?20:(i%5==0)?15:10;

        painter->drawLine(point.x(),point.y(),point.x()-linelen,point.y());
        if(i%10 ==0){
            int textY = i==0?-5: i==180 ?-15:-10;
            painter->drawText(point.x()-40,point.y()+textY,20,20,Qt::AlignCenter,QString("%1").arg(i));
            painter->drawText(point.x()-60,point.y()+textY,20,20,Qt::AlignCenter,QString("%1").arg(180-i));
            if(i!=0&&i!=180)
                painter->drawLine(point.x()-65,point.y(),point.x()-boundingRect().width()/2+40,point.y());

        }
        painter->restore();
    }

    //底部边线
    painter->drawLine(0,boundingRect().height(),boundingRect().width(),boundingRect().height());
    // 绘制中心点
    painter->setBrush(Qt::red);
    painter->drawEllipse(boundingRect().width()/2-2.5,boundingRect().height()-5,5,5);

}

QPointF Protractor::adjustSize(QPointF from,QPointF to)
{
    return QPointF(0,0);
}

QVector<QPointF> Protractor::getControlButtonPos()
{
    QVector<QPointF> points;
    points.insert(0,QPointF(0,0));
    points.insert(1,QPointF(100,100));
    points.insert(2,QPointF(200,200));
    return points;
}

void Protractor::updateShape()
{
    shape_ =QPainterPath();
    QRect rect = QRect(0,0,boundingRect().width(),boundingRect().height()*2);
    shape_.moveTo(boundingRect().width()/2, boundingRect().height());
    shape_.arcTo(rect, 0.0, 180.0);
}

