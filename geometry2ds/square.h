#ifndef SQUARE_H
#define SQUARE_H

#include "rectangle.h"

class Square : public Rectangle
{
    Q_OBJECT
public:
    Q_INVOKABLE Square(Resource * res);

    Q_INVOKABLE Square(Square const & o);

    Square(QPointF const & vec1, QPointF const & vec2);

public:
    virtual void movePoint(QPointF const & pt) override;

    virtual bool moveElememt(int elem, const QPointF &pt) override;
};

#endif // SQUARE_H
