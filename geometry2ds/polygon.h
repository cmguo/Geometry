#ifndef POLYGON_H
#define POLYGON_H

#include "geometry2d.h"

class Polygon : public Geometry2D
{
    Q_OBJECT
public:
    Q_INVOKABLE Polygon(Resource * res);

    Polygon(QPointF const & pt);

    Polygon(Polygon const & o);

public:
    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;

protected:
    virtual int pointCount();

    virtual QPointF point(int index);

    virtual bool setPoint(int index, QPointF const & pt);

    virtual QPointF lastPoint(QPointF & hint);

    virtual QPointF firstPoint(QPointF & hint);

    virtual QPointF iterPoint(int index, QPointF & hint);

    virtual QPointF prevPoint(int index, QPointF & hint);

    virtual QPointF nextPoint(int index, QPointF & hint);

protected:
    bool moveKeepAngle(int elem, const QPointF &pt);

    qreal angle(int index);

    void addAngleLabeling(QPainterPath & path, int index);

    void addAngleLabeling(QPainterPath & path, int index, qreal angle);

protected:
    static qreal angle(QPointF const & p1, QPointF const & p2, QPointF const & p3);

    static QPointF crossPoint(QPointF const & p1, QPointF const & p2,
                              QPointF const & q1, QPointF const & q2);

    static void moveLine(QPointF const & llpt, QPointF & lpt, QPointF const & pt,
                    QPointF & npt, QPointF const & nnpt);

    static void addAngleLabeling(QPainterPath & path, QPointF const & lpt, QPointF const & pt,
                          QPointF const & npt);

    static void addAngleLabeling(QPainterPath & path, QPointF const & lpt, QPointF const & pt,
                          QPointF const & npt, qreal angle);

};

#endif // POLYGON_H
