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
    virtual QPainterPath path() override;

private:
    void arcTo(QPainterPath & path, const QPointF &point, const QSizeF &size, double rotationAngle, bool isLargeArc, bool sweepDirection);
};

#endif // ARC_H
