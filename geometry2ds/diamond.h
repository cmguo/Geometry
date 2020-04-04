#ifndef DIAMOND_H
#define DIAMOND_H

#include "polygon.h"

class Diamond : public Polygon
{
    Q_OBJECT
public:
    Q_INVOKABLE Diamond(Resource * res);

    Q_INVOKABLE Diamond(Diamond const & o);

public:
    virtual int pointCount() override;

    virtual QPointF point(int index) override;

    virtual bool setPoint(int index, const QPointF &pt) override;
};


#endif // DIAMOND_H
