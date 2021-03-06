#ifndef CIRCLE_H
#define CIRCLE_H

#include "ellipse.h"

class Circle : public Ellipse
{
    Q_OBJECT
public:
    Q_INVOKABLE Circle(Resource * res);

    Q_INVOKABLE Circle(Circle const & o);

    Circle(QPointF const & center, qreal radius);

public:
    virtual QPainterPath graphPath() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool moveElememt(int elem, QPointF const & pt) override;
};

#endif // CIRCLE_H
