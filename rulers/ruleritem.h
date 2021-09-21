#ifndef RULERITEM_H
#define RULERITEM_H

#include <QGraphicsItem>

class RulerItem : public QGraphicsItem
{
public:
    explicit RulerItem(QGraphicsItem *parent = nullptr);

    RulerItem(qreal width, qreal height, QGraphicsItem *parent = nullptr);

    ~RulerItem() override;

protected:
    virtual QRectF boundingRect()const override;

    virtual QPainterPath shape() const override;

    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    enum TickFlags {
        Anticlockwise = 1,
        CrossLittenEndian = 2,
        NeedRotate = 4,
        ClipByShape = 8
    };

    // 刻度线，
    QVector<QPointF> tickMarkPoints(QPointF const & from, QPointF const & to, int flags);

    void drawTickMarks(QPainter * painter, QPointF const & from, QPointF const & to, int flags);

protected:
    virtual QPointF adjust(QPointF const & offset);

    // Unit length vector
    virtual QPointF adjustDirection(QRectF & adjust);

    virtual void updateShape();

    virtual QVector<QPointF> getControlButtonPos() = 0;

    virtual void onDraw(QPainter * painter);

protected:
    static constexpr qreal Unit = 5; // 每毫米px

    void adjustControlButtonPos();

    QGraphicsItem * iconItem(QString const & url);

protected:
    qreal width_;
    qreal height_;
    qreal minWidth_ = 300;
    QPointF rotateCenter_;
    QPainterPath shape_;
    QPainterPath shape1_;
    QPainterPath shape2_;

private:
    QGraphicsItem * deleteItem_ = nullptr;
    QGraphicsItem * adjustItem_ = nullptr;
    QGraphicsItem * rotateItem_ = nullptr;
    bool isPressed = false;
    QPointF lastPoint_;
};
#endif // RULERITEM_H
