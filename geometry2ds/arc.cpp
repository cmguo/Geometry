#include "arc.h"
#include "geometryhelper.h"

#include <QtMath>

Arc::Arc(Resource * res)
    : Geometry2D(res)
{
}

Arc::Arc(Arc const & o)
    : Geometry2D(o)
{
}

QPainterPath Arc::path()
{
    QPainterPath ph;
    if (points_.size() > 1)
    {
        ph.moveTo(points_[1]);
        ph.lineTo(points_[0]);
        arcTo(ph, points_[1], QSizeF(400, 200), 45, false, false);
    }
    return ph;
}

void Arc::arcTo(QPainterPath & path, const QPointF &point, const QSizeF &size, double rotationAngle, bool isLargeArc, bool sweepDirection)
{
    // pt1, pt2 are two points in unit circle (location at [0, 0] and with r = 1)
    //   pt1 = [cos(t1) , sin(t1)]
    //   pt2 = [cos(t2) , sin(t2)]
    // pe1, pe2 are pt1, pt2 scaled by [rx, ry] = size, that is in non-rotate Arc location at [0, 0]
    //   pe1 = [rx * cos(t1), ry * sin(t1)]
    //   pe2 = [rx * cos(t2), ry * sin(t2)]
    // pa1, pa2 are pe1, pe2 rotate by r = sweepDirection, that is in rotate Arc location at [0, 0]
    //   |pa1|   |cos(t1) sin(t1)|   |rx  0|   | cos(r) sin(r)|
    //   |   | = |               | * |     | * |              |
    //   |pa2|   |cos(t2) sin(t2)|   |0  ry|   |-sin(r) cos(r)|
    rotationAngle = rotationAngle * M_PI / 180;
    QPointF rotate(cos(rotationAngle), sin(rotationAngle)); // [cos(r), sin(r)]
    qreal rx = size.width(), ry = size.height();
    QMatrix matrix(rx * rotate.x(), rx * rotate.y(),
                -ry * rotate.y(), ry * rotate.x(), 0, 0);
    // t = [pt1 - pt2] / 2 = [cos(t1) - cos(t2), sin(t1) - sin(t2)] / 2;
    //                     = [-sin((t1 + t2) / 2) * sin((t1 - t2) / 2), cos((t1 + t2) / 2) * sin((t1 - t2) / 2)]
    QPointF t = matrix.inverted().map(path.currentPosition() - point) / 2;
    qDebug() << "t" << t;
    // a1 = (t1 + t2) / 2, a2 = (t1 - t2) / 2; t1 > t2
    qreal a1 = atan(-t.x() / t.y());
    qreal a2 = asin(sqrt(GeometryHelper::length2(t)));
    qDebug() << "a1 <-> a2" << (a1 * 180 / M_PI) << (a2 * 180 / M_PI);
    if (isLargeArc != sweepDirection)
        a2 = -a2;
    if (a2 * t.y() <= 0)
        a1 += M_PI;
    qreal t1 = a1 + a2, t2 = a1 - a2;
    qDebug() << "t1 <-> t2" << (t1 * 180 / M_PI) << (t2 * 180 / M_PI);
    QPointF pt1(cos(t1), sin(t1));
    QPointF pt2(cos(t2), sin(t2));
    qDebug() << "pt1 <-> pt2" << pt1 << pt2;
    qDebug() << "t" << (pt1 - pt2) / 2;
    QPointF c = point - matrix.map(pt2);
    //
    QRectF rect(-rx, -ry, rx * 2, ry * 2);
    QPointF pe1(pt1.x() * rx, pt1.y() * ry);
    QPointF pe2(pt2.x() * rx, pt2.y() * ry);
    qDebug() << "pe1 <-> pe2" << pe1 << pe2;
    a1 = 360 - t1 * 180 / M_PI;//angle(pe1); // Y axis is up
    a2 = 360 - t2 * 180 / M_PI;//angle(pe2);
    qDebug() << "a1 <-> a2" << a1 << a2;
    QPainterPath ph(pe1);
    a2 -= a1;
    if (sweepDirection) {
        if (a2 > 0)
            a2 -= 360;
    } else {
        if (a2 < 0)
            a2 += 360;
    }
    ph.arcTo(rect, a1, a2);
    path.addPath(QMatrix(rotate.x(), rotate.y(), -rotate.y(), rotate.x(), c.x(), c.y()).map(ph));
    path.addEllipse(c, 4, 4);
    qDebug();
}

