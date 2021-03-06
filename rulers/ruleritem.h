#ifndef RULERITEM_H
#define RULERITEM_H

#include <QGraphicsItem>

class Ruler;
class Control;

class RulerItem : public QGraphicsItem
{
public:
    explicit RulerItem(Ruler * ruler, QGraphicsItem *parent = nullptr);

    virtual ~RulerItem() override;

protected:
    virtual QRectF boundingRect()const override;

    virtual QPainterPath shape() const override;

    virtual bool sceneEvent(QEvent *event) override;

    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void updateShape();

    void adjustControlPositions();

    QGraphicsItem * iconItem(QString const & url);

protected:
    Ruler * ruler_;
    // drawing
    Control * geometry_ = nullptr;

private:
    QGraphicsItem * deleteItem_ = nullptr;
    QGraphicsItem * adjustItem_ = nullptr;
    QGraphicsItem * rotateItem_ = nullptr;
    bool isPressed = false;
    QPointF lastPoint_;
};
#endif // RULERITEM_H
