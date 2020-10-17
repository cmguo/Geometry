#ifndef ARC_H
#define ARC_H

#include "geometry2d.h"

class Arc : public Geometry2D
{
    Q_OBJECT
public:
    Q_INVOKABLE Arc(Resource * res);

    Q_INVOKABLE Arc(Arc const & o);

public:
    virtual QPainterPath graphPath() override;

private:
    void arcTo(QPainterPath & graphPath, const QPointF &point, const QSizeF &size, double rotationAngle, bool isLargeArc, bool sweepDirection);
};

#endif // ARC_H
