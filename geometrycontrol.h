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

    virtual QString toolsString(QString const & parent) const override;

    virtual void getToolButtons(QList<ToolButton *> &buttons, const QList<ToolButton *> &parents = {}) override;

    virtual void updateToolButton(ToolButton * button) override;

protected:
    virtual void updateTransform() override;

    virtual SelectMode selectTest(QPointF const & point) override;

    virtual void select(bool selected) override;

protected:
    void setPen(QPen const & pen);

    void updateGraph();

    void finishGraph();

    QRectF bounds();

protected:
    virtual bool event(QEvent *event) override;

private:
    void updateSettings();

private:
    int hitElem_;
    QPointF hitOffset_;
    QVector<QPointF> editPoints_;
    bool editing_;
};

#endif // GRAPHCONTROL_H
