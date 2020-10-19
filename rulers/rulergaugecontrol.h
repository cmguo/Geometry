#ifndef RULERGAUGECONTROL_H
#define RULERGAUGECONTROL_H

#include <core/control.h>
#include "Geometry_global.h"

class GEOMETRY_EXPORT RulerGaugeControl: public Control
{
    Q_OBJECT

public:
    Q_INVOKABLE RulerGaugeControl(ResourceView *res);

    virtual QString toolsString(QByteArray const & parent) const override;

    using :: Control::sizeChanged;

protected:
    virtual ControlView * create(ControlView * parent) override;

    virtual void attaching() override;

    virtual void attached() override;
};

#endif // RULERGAUGECONTROL_H
