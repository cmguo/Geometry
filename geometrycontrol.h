#ifndef GRAPHCONTROL_H
#define GRAPHCONTROL_H

#include <core/control.h>

#include <QColor>
#include <QPointF>
#include <QVector>

class Geometry;

class GeometryControl : public Control
{
    Q_OBJECT
public:
    Q_INVOKABLE GeometryControl(ResourceView * res, Flags flags = None, Flags clearFlags = None);

public slots:
    void setColor(QColor color);

    void setLineWidth(qreal width);

    void edit();

protected:
    virtual QGraphicsItem * create(ResourceView * res) override;

    virtual void attached() override;

    virtual void resize(QSizeF const & size) override;

    virtual QString toolsString(QString const & parent) const override;

    virtual void getToolButtons(QList<ToolButton *> &buttons, const QList<ToolButton *> &parents = {}) override;

    virtual void updateToolButton(ToolButton * button) override;

protected:
    virtual SelectMode selectTest(QPointF const & point) override;

    virtual void select(bool selected) override;

protected:
    void setPen(QPen const & pen);

    // update new path to item
    //  also update edit points when editing
    void updateGeometry();

    // finish drawing or adjusting geometry
    //  will adjust center & update
    void finishGeometry();

    QRectF bounds();

protected:
    virtual bool event(QEvent *event) override;

private:
    void updateSettings();

    void geometryChanged();

private:
    int hitElem_;
    QPointF hitOffset_;
    QPointF hitStart_;
    bool hitMoved_;
    QVector<QPointF> editPoints_;
    bool editing_;
};

#endif // GRAPHCONTROL_H
