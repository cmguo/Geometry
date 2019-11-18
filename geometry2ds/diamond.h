#ifndef DIAMOND_H
#define DIAMOND_H

#include "parallelogram.h"

class Diamond : public Parallelogram
{
    Q_OBJECT
public:
    Q_INVOKABLE Diamond(Resource * res);

    Diamond(QPointF const & pt);

    Diamond(Diamond const & o);

public:
    virtual QPointF point(int index) override;

    virtual bool setPoint(int index, const QPointF &pt) override;
};


#endif // DIAMOND_H
