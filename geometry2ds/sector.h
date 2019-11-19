#ifndef SECTOR_H
#define SECTOR_H

#include "geometry2d.h"

class Sector : public Geometry2D
{
    Q_OBJECT
public:
    Q_INVOKABLE Sector(Resource * res);

    Sector(QPointF const & pt);

    Sector(Sector const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool move(int elem, QPointF const & pt) override;

private:
    qreal angle_ = 0.0;
};

#endif // SECTOR_H
