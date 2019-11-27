#ifndef CIRCLE_H
#define CIRCLE_H

#include "ellipse.h"

class Circle : public Ellipse
{
    Q_OBJECT
public:
    Q_INVOKABLE Circle(Resource * res);

    Circle(QPointF const & pt);

    Q_INVOKABLE Circle(Circle const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool move(int elem, QPointF const & pt) override;
};

#endif // CIRCLE_H
