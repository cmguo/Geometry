#ifndef SECTOR_H
#define SECTOR_H

#include "geometry2d.h"

class Sector : public Geometry2D
{
    Q_OBJECT

    Q_PROPERTY(qreal angle READ angle WRITE setAngle)

public:
    Q_INVOKABLE Sector(Resource * res);

    Sector(QPointF const & pt);

    Q_INVOKABLE Sector(Sector const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool move(int elem, QPointF const & pt) override;

public slots:
    qreal angle() { return qAbs(angle_); }

    void setAngle(qreal angle);

    void draw(QPainter *painter);

private:
    virtual void getToolButtons(QList<ToolButton *> & buttons,
                                ToolButton * parent) override;

    virtual void updateToolButton(ToolButton * button) override;

private:
    qreal angle_ = 0.0;
    QPainterPath path_;
    QPainterPath textPath_;
};

#endif // SECTOR_H
