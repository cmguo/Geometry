#ifndef GEOMETRYHELPER_H
#define GEOMETRYHELPER_H

#include <QPointF>
#include <QVector>
#include <QPolygonF>

class GeometryHelper
{
public:
    static constexpr qreal HIT_DIFF = 10.0;

    static constexpr qreal HIT_DIFF_DIFF = 100.0;

public:
    static qreal angle(QPointF const & vec);

    static qreal angle(QPointF const & p1, QPointF const & p2, QPointF const & p3);

    static qreal length(QPointF const & vec);

    static qreal length2(QPointF const & vec);

    static qreal determinant(QPointF const & p1, QPointF const & p2);

    static qreal dotProduct(QPointF const & p1, QPointF const & p2);

public:
    static void rotate(QPointF & pt, QPointF const & angle);

    static void reverseRotate(QPointF & pt, QPointF const & angle);

    static void adjustToLength(QPointF const & start, QPointF & end, qreal length);

public:
    static int attachToPoints(QVector<QPointF> const & pts, QPointF & p);

    static void attachToLine(QPointF const & p1, QPointF const & p2, QPointF & p);

    static void attachToLines(QPointF const & p1, QPointF & p);

    static void attachToLines(QPointF const & p1, QPointF const & p2, QPointF & p);

    static int attachToLines(QPointF const & p1, QVector<QPointF> const & dirs, QPointF & p);

    static int attachToLines(QPointF const & p1, QPointF const & p2, QVector<QPointF> const & dirs, QPointF & p);

    static int attachLineToPointsAndLines(QVector<QPointF> const & pts, QVector<int> lines, QPointF & lp1, QPointF & lp2);

public:
    static qreal dist2PointToSegment(QPointF const & p1, QPointF const & p2,
                                    QPointF const & p, QPointF & rp);

    static QPointF nearestPointAtVerticalBisector(QPointF const & p1, QPointF const & p2,
                                   QPointF const & p);

    static QPointF crossPoint(QPointF const & p1, QPointF const & p2,
                              QPointF const & q1, QPointF const & q2);

public:
    static QPolygonF smallestEnclosingPolygon(QVector<QPointF> const & pts);
};

#endif // GEOMETRYHELPER_H