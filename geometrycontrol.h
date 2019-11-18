#ifndef GRAPHCONTROL_H
#define GRAPHCONTROL_H

#include <core/control.h>

#include <QColor>

class Geometry;

class GeometryControl : public Control
{
    Q_OBJECT
public:
    Q_INVOKABLE GeometryControl(ResourceView * res, Flags flags = None, Flags clearFlags = None);

public slots:
    void setColor(QColor color);

    void setLineWidth(qreal width);

protected:
    virtual QGraphicsItem * create(ResourceView * res) override;

    virtual void attached() override;

    virtual QString toolsString() const override;

protected:
    virtual void updateTransform() override;

    virtual SelectMode selectTest(QPointF const & point) override;

protected:
    virtual void setPen(QPen const & pen);

    virtual void updateGraph(Geometry * gh);

    virtual QRectF bounds();

protected:
    virtual bool event(QEvent *event) override;

private:
    void updateSettings();

private:
    int hitElem_;
    QPointF hitDiff_;
};

#endif // GRAPHCONTROL_H
