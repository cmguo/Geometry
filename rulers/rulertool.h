#ifndef RULERTOOL_H
#define RULERTOOL_H

#include <core/control.h>

class RulerTool: public Control
{
    Q_OBJECT
public:
    Q_INVOKABLE RulerTool(ResourceView *res);

    virtual QString toolsString(QByteArray const & parent) const override;

    using :: Control::sizeChanged;

protected:
    virtual ControlView * create(ControlView * parent) override;

    virtual void attaching() override;

    virtual void attached() override;
};

#endif // RULERTOOL_H
