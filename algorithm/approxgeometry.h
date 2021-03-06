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

    Geometry * approxGeometry4();

    Geometry * approxGeometryRegular();

    Geometry * approxGeometryEllipse();

private:
    static void RamerDouglasPeucker(const QVector<QPointF> &pointList, qreal epsilon, QVector<QPointF> &out);

    static QVector<qreal> edgeLengths(QVector<QPointF> const & points);

    static QVector<qreal> edgeAngles(QVector<QPointF> const & points, QVector<qreal> const & lengths);

private:
    qreal epsilon_;
    QVector<QPointF> points_;
    qreal rotate_ = 0;
};

#endif // APPROXGEOMETRY_H
