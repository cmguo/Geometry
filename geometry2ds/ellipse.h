#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "geometry2d.h"

class Ellipse : public Geometry2D
{
    Q_OBJECT
public:
    Q_INVOKABLE Ellipse(Resource * res);

    Q_INVOKABLE Ellipse(Ellipse const & o);

    Ellipse(QPointF const & center, QSizeF const & radius);

public:
    virtual QPainterPath graphPath() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;
};

#endif // ELLIPSE_H
