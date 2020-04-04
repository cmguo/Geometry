#ifndef SQUARE_H
#define SQUARE_H

#include "rectangle.h"

class Square : public Rectangle
{
    Q_OBJECT
public:
    Q_INVOKABLE Square(Resource * res);

    Q_INVOKABLE Square(Square const & o);

public:
    virtual void movePoint(QPointF const & pt) override;

    virtual bool move(int elem, const QPointF &pt) override;
};

#endif // SQUARE_H
