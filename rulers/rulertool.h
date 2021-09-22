#ifndef RULERTOOL_H
#define RULERTOOL_H

#include <core/control.h>

class Geometry;

class RulerTool: public Control
{
    Q_OBJECT
public:
    Q_INVOKABLE RulerTool(ResourceView *res);

    virtual QString toolsString(QByteArray const & parent) const override;

    using :: Control::sizeChanged;

public:
    Control * addGeometry(Geometry * geometry);

    void finishGeometry(Control * geometry);

protected:
    virtual ControlView * create(ControlView * parent) override;

    virtual void attaching() override;

    virtual void attached() override;

    virtual SelectMode selectTest(ControlView *child, ControlView *parent, const QPointF &point, bool onlyAssist) override;
};

#endif // RULERTOOL_H
