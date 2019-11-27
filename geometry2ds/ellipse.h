#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "geometry2d.h"

class Ellipse : public Geometry2D
{
    Q_OBJECT
public:
    Q_INVOKABLE Ellipse(Resource * res);

    Ellipse(QPointF const & pt);

    Q_INVOKABLE Ellipse(Ellipse const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;
};

#endif // ELLIPSE_H
