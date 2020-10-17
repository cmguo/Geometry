#ifndef SECTOR_H
#define SECTOR_H

#include "geometry2d.h"

class Sector : public Geometry2D
{
    Q_OBJECT

    Q_CLASSINFO("toolsString", "angle|角度|Popup,OptionsGroup,NeedUpdate|;")

    Q_PROPERTY(qreal angle READ angle WRITE setAngle)

public:
    Q_INVOKABLE Sector(Resource * res);

    Q_INVOKABLE Sector(Sector const & o);

public:
    virtual QPainterPath graphPath() override;

    virtual QPainterPath textPath() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool move(int elem, QPointF const & pt) override;

    virtual void sync() override;

public:
    qreal angle();

    void setAngle(qreal angle);

private:
    qreal angle_ = 0.0;
    QPainterPath graphPath_;
    QPainterPath textPath_;
};

#endif // SECTOR_H
