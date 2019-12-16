#ifndef RULERGAUGEBASE_H
#define RULERGAUGEBASE_H

#include <QGraphicsItem>



class  RulerGaugeBase : public QGraphicsItem
{
public:
    explicit RulerGaugeBase(QGraphicsItem *parent = nullptr);
    RulerGaugeBase(int width,int height,QGraphicsItem *parent = nullptr);
    ~RulerGaugeBase();

public:
    QRectF boundingRect()const override;
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void rotation();
    virtual void adjustSize();
    int getWidth() const;
    int getHeight() const;

protected:
    virtual void paintCalibration(QPainter *painter,int length);

protected:
    int width_;
    int height_;
    const int PxPerMilliMeter = 10;// 每毫米px
    QGraphicsPixmapItem *adjustSizeItem = nullptr;
    QGraphicsPixmapItem *rotationItem = nullptr;
    bool isPressed = false;

};
#endif // RULERGAUGEBASE_H
