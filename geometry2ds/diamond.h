#ifndef DIAMOND_H
#define DIAMOND_H

#include "polygon.h"

class Diamond : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE Diamond(Resource * res);

    Q_INVOKABLE Diamond(Diamond const & o);

    Diamond(QPointF const & vec1, QPointF const & vec2);

public:
    virtual int pointCount() override;

    virtual QPointF point(int index) override;

    virtual bool setPoint(int index, const QPointF &pt) override;
};


#endif // DIAMOND_H
