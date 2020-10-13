#ifndef GEOMETRYHELPER_H
#define GEOMETRYHELPER_H

#include "Geometry_global.h"

#include <QPointF>
#include <QVector>
#include <QPolygonF>
#include <QFont>
#include <QPainterPath>
#include <QColor>

class ResourcePage;

class GEOMETRY_EXPORT GeometryHelper : public QObject
{
    Q_OBJECT
public:
    static qreal HIT_DIFF;

    static qreal HIT_DIFF_DIFF;

    static qreal DEFAULT_LINE_WIDTH;

    static QFont TEXT_FONT;

    Q_INVOKABLE GeometryHelper();

public:
    static QColor defaultColor(ResourcePage * page);

    static void setDefaultColor(ResourcePage * page, QColor color);

public slots:
    static void init();

    static QPointF textOffset(QString const & text, Qt::Alignment alignment);

    static QPainterPath toRoundPolygon(QPolygonF const & polygon, qreal radius);

    static QPainterPath toRoundPolygon(QPolygonF const & polygon, QVector<qreal> const & radiuses);

public slots:
    static qreal angle(QPointF const & vec);

    static qreal angle(QPointF const & p1, QPointF const & p2, QPointF const & p3);

    static qreal length(QPointF const & vec);

    static qreal length2(QPointF const & vec);

    static qreal determinant(QPointF const & p1, QPointF const & p2);

    static qreal dotProduct(QPointF const & p1, QPointF const & p2);

public slots:
    static void rotate(QPointF & pt, QPointF const & angle);

    static void reverseRotate(QPointF & pt, QPointF const & angle);

    static void adjustToLength(QPointF & vec, qreal length);

    static void adjustToLength(QPointF const & start, QPointF & end, qreal length);

public slots:
    static int attachToPoints(QVector<QPointF> const & pts, QPointF & p);

    static void attachToLine(QPointF const & p1, QPointF const & p2, QPointF & p);

    static void attachToLines(QPointF const & p1, QPointF & p);

    static void attachToLines(QPointF const & p1, QPointF const & p2, QPointF & p);

    static int attachToLines(QPointF const & p1, QVector<QPointF> const & dirs, QPointF & p);

    static int attachToLines(QPointF const & p1, QPointF const & p2, QVector<QPointF> const & dirs, QPointF & p);

    static int attachLineToPointsAndLines(QVector<QPointF> const & pts, QVector<int> lines, QPointF & lp1, QPointF & lp2);

public slots:
    static qreal dist2PointToSegment(QPointF const & p1, QPointF const & p2,
                                    QPointF const & p, QPointF & rp);

    static QPointF nearestPointAtVerticalBisector(QPointF const & p1, QPointF const & p2,
                                   QPointF const & p);

    static QPointF crossPoint(QPointF const & p1, QPointF const & p2,
                              QPointF const & q1, QPointF const & q2);

public slots:
    static QPolygonF smallestEnclosingPolygon(QVector<QPointF> const & pts);
};

#endif // GEOMETRYHELPER_H
