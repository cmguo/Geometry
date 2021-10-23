#include "approxgeometry.h"

#include <base/geometryhelper.h>

#include <geometry2ds/diamond.h>
#include <geometry2ds/isopopetrapezoid.h>
#include <geometry2ds/isoscelestriangle.h>
#include <geometry2ds/parallelogram.h>
#include <geometry2ds/rectangle.h>
#include <geometry2ds/regularpolygon.h>
#include <geometry2ds/righttrapezoid.h>
#include <geometry2ds/righttriangle.h>
#include <geometry2ds/square.h>

#include <core/resource.h>
#include <core/resourcetransform.h>

ApproxGeometry::ApproxGeometry(qreal epsilon)
    : epsilon_(epsilon)
{
}

static qreal corvelength(QVector<QPointF> const & points)
{
    std::pair<QPointF const *,qreal> sum = {nullptr, 0};
    return std::accumulate(points.begin(), points.end(), sum, [](auto sum, QPointF const & p) {
        if (sum.first) {
            sum.second += GeometryHelper::length(p - *sum.first);
        }
        sum.first = &p;
        return sum;
    }).second;
}

bool ApproxGeometry::addPoints(QVector<QPointF> points)
{
    qreal epsilon = epsilon_;
    if (epsilon == 0) {
        epsilon = 0.03 * corvelength(points);
    }
    QVector<QPointF> result;
    RamerDouglasPeucker(points, epsilon, result);
    if (points_.empty()) {
        points_.append(result);
    } else {
        if (GeometryHelper::length(points_.front() - result.front()) < epsilon) {
            result.pop_front();
            for (auto & p : result)
                points_.prepend(p);
        } else if (GeometryHelper::length(points_.front() - result.back()) < epsilon) {
            result.pop_back();
            result.append(points_);
            points_ = result;
        } else if (GeometryHelper::length2(points_.back() - result.back()) < epsilon) {
            result.pop_back();
            while (!result.empty()) {
                points.append(result.back());
                result.removeLast();
            }
        } else if (GeometryHelper::length2(points_.back() - result.front()) < epsilon) {
            result.pop_front();
            points_.append(result);
        } else {
            points_ = result;
            return false;
        }
    }
}

QObject *ApproxGeometry::finish(const QPointF &offset)
{
    if (points_.size() < 3)
        return nullptr;
    qreal epsilon = epsilon_;
    if (epsilon == 0)
        epsilon = 0.05 * corvelength(points_);
    if (GeometryHelper::length(points_.back() - points_.front()) < epsilon) {
        points_.pop_back();
        for (auto & p : points_) {
            p += offset;
        }
        Geometry * geometry = nullptr;
        if (points_.size() == 3) {
            geometry = approxGeometry3();
        } else if (points_.size() == 4) {
            geometry = approxGeometry4();
        }
        if (geometry == nullptr)
            geometry = new Polygon(points_);
        geometry->sync();
        geometry->finish(geometry->contour().boundingRect().center());
        geometry->sync();
        if (rotate_ != 0) {
            geometry->transform().rotate(rotate_);
        }
        return geometry;
    }
    return nullptr;
}

static bool approxEqual(qreal l, qreal r)
{
    qreal s = l / r;
    return s > 0.9 && s < 1.11;
}

static qreal smallRatio(qreal l, qreal r)
{
    return l < r ? l / r : r / l;
}

Geometry *ApproxGeometry::approxGeometry3()
{
    // length of each edges
    qreal l1 = GeometryHelper::length(points_[1] - points_[0]);
    qreal l2 = GeometryHelper::length(points_[2] - points_[1]);
    qreal l3 = GeometryHelper::length(points_[0] - points_[2]);
    // approx ratio with edges (always <= 1)
    qreal e12 = smallRatio(l1, l2);
    qreal e23 = smallRatio(l2, l3);
    qreal e31 = smallRatio(l3, l1);
    // center point
    QPointF center = std::accumulate(points_.begin(), points_.end(), QPointF{0, 0}) / 3;
    // all ratio > 0.9, regular triangle
    if (e12 > 0.9 && e23 > 0.9 && e31 > 0.9) {
        return new RegularPolygon(center, points_[0], 3);
    }
    // cos() of each angle
    qreal a12 = qAbs(GeometryHelper::dotProduct(points_[1] - points_[0], points_[2] - points_[1])) / l1 / l2;
    qreal a23 = qAbs(GeometryHelper::dotProduct(points_[2] - points_[1], points_[0] - points_[2])) / l2 / l3;
    qreal a31 = qAbs(GeometryHelper::dotProduct(points_[0] - points_[2], points_[1] - points_[0])) / l3 / l1;
    // most close to 90° of each angle
    qreal amin = std::min(a12, a23);
    int imin = a12 > a23 ? 2 : 1;
    if (a31 < amin) {
        imin = 0;
        amin = a31;
    }
    // maximun length ratio, length ratio of two approx equal edge
    qreal emax = std::max(e12, e23);
    int imax = e12 < e23 ? 2 : 1;
    if (e31 > emax) {
        imax = 0;
        emax = e31;
    }
    // IsoscelesTriangle
    if (emax > 0.9) {
        QPointF point = points_[imax];
        QPointF middle = (center * 3 - point) / 2; // botton middle
        qreal angle = GeometryHelper::angle(point - middle) - 90;
        QTransform tr; tr.rotate(angle);
        QPointF point2 = tr.map(point - middle) / 2;
        QPointF middle2 = -point2;
        qreal length = imax == 1 ? l3 : (imax == 2 ? l1 : l2);
        if (imax == imin && amin < 0.15) { // 8.6° IsoscelesRightTriangle
            length = point2.y() * 4;
        }
        qDebug() << "approxGeometry3" << point2 << middle2 << length;
        middle2.setX(length / 2);
        center = (middle + point) / 2; //
        rotate_ = -angle;
        return new IsoscelesTriangle(point2 + center, middle2 + center);
    }
    // RightTriangle
    if (amin < 0.15) { // 8.6°
        QPointF point = points_[imin];
        QPointF pointa = imin == 0 ? points_[1] : points_[0];
        QPointF pointb = imin == 2 ? points_[1] : points_[2];
        QPointF point2 = pointb - pointa; point2 = {point2.y(), -point2.x()};
        if (GeometryHelper::dotProduct(point2, point - pointa) > 0)
            std::swap(pointa, pointb);
        GeometryHelper::dist2PointToSegment(pointa, pointb, point, point2);
        qreal lengtha = GeometryHelper::length(point2 - pointa);
        qreal lengthb = GeometryHelper::length(point2 - pointb);
        QPointF point3{sqrt(lengthb * (lengtha + lengthb)), sqrt(lengtha * (lengtha + lengthb))};
        rotate_ = GeometryHelper::angle(point3) - GeometryHelper::angle(pointb - pointa);
        qDebug() << "approxGeometry3" << pointa << pointb << point3;
        point2 = (pointa + pointb) / 2;
        return new RightTriangle(point2 - point3 / 2, point2 + point3 / 2);
    }
    return nullptr;
}

static qreal maximun(qreal l, qreal r)
{
    return std::max(l, r);
}

static qreal minimun(qreal l, qreal r)
{
    return std::min(l, r);
}

Geometry *ApproxGeometry::approxGeometry4()
{
    // length of each edges
    std::array<qreal, 4> lengths = {
        GeometryHelper::length(points_[1] - points_[0]),
        GeometryHelper::length(points_[2] - points_[1]),
        GeometryHelper::length(points_[3] - points_[2]),
        GeometryHelper::length(points_[0] - points_[3])
    };
    qDebug() << "approxGeometry4 length of each edges" << QVector<qreal>(lengths.begin(), lengths.end());
    // cos() of each angle
    std::array<qreal, 4> angles = {
        qAbs(GeometryHelper::dotProduct(points_[1] - points_[0], points_[2] - points_[1])) / lengths[0] / lengths[1],
        qAbs(GeometryHelper::dotProduct(points_[2] - points_[1], points_[3] - points_[2])) / lengths[1] / lengths[2],
        qAbs(GeometryHelper::dotProduct(points_[3] - points_[2], points_[0] - points_[3])) / lengths[2] / lengths[3],
        qAbs(GeometryHelper::dotProduct(points_[0] - points_[3], points_[1] - points_[0])) / lengths[3] / lengths[0]
    };
    qDebug() << "approxGeometry4 cos() of each angle" << QVector<qreal>(angles.begin(), angles.end());
    // angles of both side edges (cos())
    qreal a13 = GeometryHelper::dotProduct(points_[1] - points_[0], points_[3] - points_[2]) / lengths[0] / lengths[2];
    qreal a24 = GeometryHelper::dotProduct(points_[2] - points_[1], points_[0] - points_[3]) / lengths[3] / lengths[1];
    qDebug() << "approxGeometry4 angles of both side edges (cos())" << a13 << a24;
    if (a13 < -0.9 && a24 < -0.9) { // 5.7°
        // Parallelogram
        QPointF middle = (points_[0] + points_[2]) / 2;
        QPointF v01 = points_[1] - points_[0];
        qreal cos = GeometryHelper::dotProduct(v01, QPointF(1, 0)) / lengths[0];
        qreal sin = GeometryHelper::dotProduct(v01, QPointF(0, 1)) / lengths[0];
        QTransform tr(cos, -sin, sin, cos, 0, 0);
        QPointF p0 = tr.map(points_[0] - middle) + middle;
        QPointF p2 = middle * 2 - p0;
        rotate_ = -GeometryHelper::angle(v01);
        if (std::accumulate(angles.begin(), angles.end(), 0.0, maximun) < 0.15) { // 8.6°
            // Rectangle
            if (smallRatio(lengths[0] + lengths[2], lengths[1] + lengths[3]) > 0.9) {
                return new Square(p0, p2);
            }
            return new Rectangle(p0, p2);
        } else {
            if (smallRatio(lengths[0], lengths[1]) > 0.9) {
                QPointF v31 = points_[1] - points_[3];
                qreal length = GeometryHelper::length(v31);
                qreal cos = GeometryHelper::dotProduct(v31, QPointF(1, 0)) / length;
                qreal sin = GeometryHelper::dotProduct(v31, QPointF(0, 1)) / length;
                QTransform tr(cos, -sin, sin, cos, 0, 0);
                QPointF p0 = tr.map(points_[0] - middle) + middle;
                QPointF p1 = tr.map(points_[1] - middle) + middle;
                rotate_ = -GeometryHelper::angle(v31);
                return new Diamond(p0, p1);
            }
            QPointF p1 = tr.map(points_[1] - middle) + middle;
            return new Parallelogram(p0, p1, p2);
        }
    } else if (a13 < -0.9 || a24 < -0.9) { // 5.7°
        // Trapezoid
        QPointF v = (a13 < -0.9) ? points_[1] - points_[0] : points_[2] - points_[1];
        qreal length = (a13 < -0.9) ? lengths[0] : lengths[1];
        qreal cos = GeometryHelper::dotProduct(v, QPointF(1, 0)) / length;
        qreal sin = GeometryHelper::dotProduct(v, QPointF(0, 1)) / length;
        QTransform tr(cos, -sin, sin, cos, 0, 0);
        rotate_ = -GeometryHelper::angle(v);
        qreal const * amin = nullptr;
        amin = std::accumulate(angles.begin(), angles.end(), amin, [](auto min, qreal const & a) {
            return (min == nullptr || a < *min) ? &a : min;
        });
        int idx = amin - &angles.front();
        int idx2 = (a13 < -0.9) ? (idx ^ 1) : 3 - idx;
        // RightTrapezoid
        if (*amin < 0.1 && angles[idx2] < 0.1) { // 5.7°
            qreal l13 = GeometryHelper::length(points_[2] - points_[0]);
            qreal l24 = GeometryHelper::length(points_[3] - points_[1]);
            QPointF middle = l13 < l24 ? (points_[3] + points_[1]) / 2 : (points_[2] + points_[0]) / 2;
            if (idx == 3 || idx2 == 3) {
                QPointF p1 = tr.map(points_[1] - middle) + middle;
                QPointF p2 = tr.map(points_[2] - middle) + middle;
                if (a13 < -0.9) {
                    QPointF p0 = tr.map(points_[0] - middle) + middle;
                    p1.setY(p0.y());
                    return new RightTrapezoid(p0, p1, p2);
                } else {
                    p2.setY(p1.y());
                    QPointF p3 = tr.map(points_[3] - middle) + middle;
                    return new RightTrapezoid(p1, p2, p3);
                }
            } else {
                QPointF p3 = tr.map(points_[3] - middle) + middle;
                QPointF p0 = tr.map(points_[0] - middle) + middle;
                if (a13 < -0.9) {
                    QPointF p2 = tr.map(points_[2] - middle) + middle;
                    p3.setY(p2.y());
                    return new RightTrapezoid(p2, p3, p0);
                } else {
                    p0.setY(p3.y());
                    QPointF p1 = tr.map(points_[1] - middle) + middle;
                    return new RightTrapezoid(p3, p0, p1);
                }
            }
        }
        qreal ratio = (a13 < -0.9) ? smallRatio(lengths[1], lengths[3]) : smallRatio(lengths[0], lengths[2]);
        // IsopopeTrapezoid
        if (ratio > 0.9) {
            QPointF middle = std::accumulate(points_.begin(), points_.end(), QPointF{0, 0}) / 4;
            QPointF p1 = tr.map(points_[1] - middle) + middle;
            QPointF p2 = tr.map(points_[2] - middle) + middle;
            if (a13 < -0.9) {
                QPointF p0 = tr.map(points_[0] - middle) + middle;
                return new IsopopeTrapezoid(p0, p1, p2);
            } else {
                QPointF p3 = tr.map(points_[3] - middle) + middle;
                return new IsopopeTrapezoid(p1, p2, p3);
            }
        }
        rotate_ = 0;
    }
    return nullptr;
}

static double PerpendicularDistance(const QPointF &pt, const QPointF &lineStart, const QPointF &lineEnd)
{
    double dx = lineEnd.x() - lineStart.x();
    double dy = lineEnd.y() - lineStart.y();

    //Normalise
    double mag = pow(pow(dx,2.0)+pow(dy,2.0),0.5);
    if(mag > 0.0)
    {
        dx /= mag; dy /= mag;
    }

    double pvx = pt.x() - lineStart.x();
    double pvy = pt.y() - lineStart.y();

    //Get dot product (project pv onto normalized direction)
    double pvdot = dx * pvx + dy * pvy;

    //Scale line direction vector
    double dsx = pvdot * dx;
    double dsy = pvdot * dy;

    //Subtract this from pv
    double ax = pvx - dsx;
    double ay = pvy - dsy;

    return pow(pow(ax,2.0)+pow(ay,2.0),0.5);
}

void ApproxGeometry::RamerDouglasPeucker(const QVector<QPointF> &pointList, qreal epsilon, QVector<QPointF> &out)
{
    if (pointList.size()<2)
        throw std::invalid_argument("Not enough points to simplify");

    // Find the point with the maximum distance from line between start and end
    double dmax = 0.0;
    size_t index = 0;
    size_t end = pointList.size()-1;
    for(size_t i = 1; i < end; i++)
    {
        double d = PerpendicularDistance(pointList[i], pointList[0], pointList[end]);
        if (d > dmax)
        {
            index = i;
            dmax = d;
        }
    }

    // If max distance is greater than epsilon, recursively simplify
    if(dmax > epsilon)
    {
        // Recursive call
        QVector<QPointF> recResults1;
        QVector<QPointF> recResults2;
        QVector<QPointF> firstLine(pointList.begin(), pointList.begin()+index+1);
        QVector<QPointF> lastLine(pointList.begin()+index, pointList.end());
        RamerDouglasPeucker(firstLine, epsilon, recResults1);
        RamerDouglasPeucker(lastLine, epsilon, recResults2);

        // Build the result list
        out.append(recResults1);
        out.pop_back();
        out.append(recResults2);
        if(out.size()<2)
            throw std::runtime_error("Problem assembling output");
    }
    else
    {
        //Just return start and end points
        out.clear();
        out.push_back(pointList[0]);
        out.push_back(pointList[end]);
    }
}
