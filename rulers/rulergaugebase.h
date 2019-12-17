#ifndef RULERGAUGEBASE_H
#define RULERGAUGEBASE_H

#include <QGraphicsItem>



class  RulerGaugeBase : public QGraphicsItem
{
public:
    explicit RulerGaugeBase(QGraphicsItem *parent = nullptr);
    RulerGaugeBase(int width,int height,QGraphicsItem *parent = nullptr);
    ~RulerGaugeBase() override;

public:

    int getWidth() const;
    int getHeight() const;

protected:
    virtual void paintCalibration(QPainter *painter,int length);
    virtual QPointF adjustSize(QPointF from,QPointF to); // 返回item需要移动的point位置
    virtual QRectF boundingRect()const override;
    virtual QPainterPath shape() const override;
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual QVector<QPointF> getControlButtonPos() = 0;
    void adjustControlButtonPos();
protected:
    int width_;
    int height_;
    QPainterPath shape_;
    const int PxPerMilliMeter = 10;// 每毫米px
    QGraphicsPixmapItem *adjustSizeItem = nullptr;
    QGraphicsPixmapItem *rotationItem = nullptr;
    bool isPressed = false;
    QPointF movePoint;
};
#endif // RULERGAUGEBASE_H
