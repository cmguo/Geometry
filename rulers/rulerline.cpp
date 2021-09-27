#include "rulerline.h"
#include "base/geometryhelper.h"

#include <core/resource.h>

RulerLine::RulerLine(const QLineF &line)
    : Line(new Resource("geometry2d", QUrl("geometry2d:line")))
    , line_(line)
{
}

void RulerLine::addPoint(const QPointF &pt)
{
    QPointF dir = line_.p2() - line_.p1();
    GeometryHelper::adjustToLength(dir, width() / 2);
    dir = QPointF{dir.y(), -dir.x()};
    line_.translate(dir);
    QPointF pt2 = attachToLine(pt);
    Line::addPoint(pt2);
}

void RulerLine::movePoint(const QPointF &pt)
{
    QPointF pt2 = attachToLine(pt);
    if (points_.size() > 1) {
        qreal d = GeometryHelper::dotProduct(pt2 - points_.front(), points_.back() - pt2);
        if (d < 0) {
            qreal d2 = GeometryHelper::dotProduct(points_.front() - pt2, points_.back() - pt2);
            if (d2 > 0) {
                points_.front() = pt2;
            } else {
                points_.back() = pt2;
            }
        }
    } else {
        Line::movePoint(pt2);
    }
}

bool RulerLine::commitPoint(const QPointF &pt)
{
    movePoint(pt);
    return true;
}

QPointF RulerLine::attachToLine(const QPointF &pt)
{
    QPointF d = line_.p2() - line_.p1();
    qreal dot1 = GeometryHelper::dotProduct(d, pt - line_.p1());
    qreal dot2 = GeometryHelper::length2(d);
    qreal r = dot1 / dot2;
    return line_.p1() + d * r;
}
