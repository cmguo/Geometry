#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "geometry3d.h"

//#define POLYHEDRON_ISOMETRIC_PROJECTION

class Polyhedron : public Geometry3D
{
    Q_OBJECT
public:
    Q_INVOKABLE Polyhedron(Resource * res);

    Polyhedron(QPointF const & pt);

    Polyhedron(Polyhedron const & o);

public:
    virtual bool commitPoint(const QPointF &pt) override;

    virtual bool canFinish() override;

    virtual QPainterPath path() override;

    virtual QVector<QPointF> movePoints() override;

    virtual int hit(QPointF &pt) override;

    virtual bool move(int elem, const QPointF &pt) override;

public slots:
    void draw(QPainter *painter);

protected:
    virtual int pointCount();

    virtual QVector3D point(int index);

    virtual bool setPoint(int index, QVector3D const & pt);

protected:
    void makeLine(int startIndex, int endIndex);

    void collect(QVector<QPointF> & points, QVector<bool> & hidden);

private:
    QVector<int> lines_;
};

#endif // POLYHEDRON_H
