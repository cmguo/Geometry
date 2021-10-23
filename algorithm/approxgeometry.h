#ifndef APPROXGEOMETRY_H
#define APPROXGEOMETRY_H

#include <QVector>

class Geometry;
class QObject;

class ApproxGeometry
{
public:
    ApproxGeometry(qreal epsilon = 0);

    bool addPoints(QVector<QPointF> points);

    QObject * finish(QPointF const & offset);

private:
    Geometry * approxGeometry3();

private:
    static void RamerDouglasPeucker(const QVector<QPointF> &pointList, qreal epsilon, QVector<QPointF> &out);

private:
    qreal epsilon_;
    QVector<QPointF> points_;
    qreal rotate_ = 0;
};

#endif // APPROXGEOMETRY_H
