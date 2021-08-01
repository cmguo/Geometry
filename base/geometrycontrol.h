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
    static constexpr Flag ImpliedEditable = CustomFlag;

    Q_INVOKABLE GeometryControl(ResourceView * res, Flags flags = None, Flags clearFlags = None);

public slots:
    void edit();

    void test();

protected:
    virtual ControlView * create(ControlView * parent) override;

    virtual void attaching() override;

    virtual void attached() override;

    virtual void resize(QSizeF const & size) override;

public:
    virtual QString toolsString(QByteArray const & parent) const override;

    virtual void getToolButtons(QList<ToolButton *> &buttons, const QList<ToolButton *> &parents = {}) override;

    virtual bool setOption(const QByteArray &key, QVariant value) override;

    virtual QVariant getOption(QByteArray const & key) override;

protected:
    using Control::getToolButtons;

    virtual SelectMode selectTest(QPointF const & point) override;

    virtual void select(bool selected) override;

protected:
    void setColor(const QColor & color);

    // update new path to item
    //  also update edit points when editing
    void updateGeometry();

    // finish drawing or adjusting geometry
    //  will adjust center & update
    void finishGeometry(bool valid = false);

protected:
    virtual bool event(QEvent *event) override;

private:
    void updateSettings();

    void geometryChanged(QByteArray const & key);

    bool beginPoint(QPointF const & point, bool fromHandle);

    void movePoint(QPointF const & point);

    bool endPoint(QPointF const & point);

private:
    int hitElem_;
    QPointF hitOffset_;
    QPointF hitStart_;
    bool hitMoved_;
    QVector<QPointF> editPoints_;
    bool editing_;
    int touchId_;
    QPointF touchPos_;
};

#endif // GRAPHCONTROL_H
