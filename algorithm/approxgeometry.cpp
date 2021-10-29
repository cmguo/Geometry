#include "approxgeometry.h"

#include <base/geometryhelper.h>

#include <geometry2ds/circle.h>
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

#include <QtMath>

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
        epsilon = 0.015 * corvelength(points);
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
    if (GeometryHelper::length(points_.back() - points_.front()) > epsilon)
        return nullptr;
    points_.pop_back();
    for (auto & p : points_) {
        p += offset;
    }
    Geometry * geometry = nullptr;
    if (points_.size() == 3) {
        geometry = approxGeometry3();
    } else if (points_.size() == 4) {
        geometry = approxGeometry4();
    } else if (points_.size() < 7) {
        geometry = approxGeometryRegular();
    } else if (points_.size() >= 8) {
        geometry = approxGeometryEllipse();
    }
    if (geometry == nullptr) {
        if (points_.size() < 7)
            geometry = new Polygon(points_);
    }
    if (geometry) {
        geometry->sync();
        geometry->finish(geometry->contour().boundingRect().center());
        geometry->sync();
        if (rotate_ != 0) {
            geometry->transform().rotate(rotate_);
        }
    }
    return geometry;
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

static qreal maximun(qreal l, qreal r)
{
    return std::max(l, r);
}

static qreal minimun(qreal l, qreal r)
{
    return std::min(l, r);
}

Geometry *ApproxGeometry::approxGeometry3()
{
    auto lengths = edgeLengths(points_);
    qDebug() << "approxGeometry3 length of each edges" << lengths;
    auto angles = edgeAngles(points_, lengths);
    qDebug() << "approxGeometry3 cos() of each angle" << angles;
    // approx ratio with edges (always <= 1)
    QVector<qreal> ratios{
        smallRatio(lengths[0], lengths[1]),
                smallRatio(lengths[1], lengths[2]),
                smallRatio(lengths[2], lengths[0]),
    };
    qDebug() << "approxGeometry3 approx ratio with edges" << ratios;
    // center point
    QPointF center = std::accumulate(points_.begin(), points_.end(), QPointF{0, 0}) / 3;
    // all ratio > 0.9, regular triangle
    if (std::accumulate(ratios.begin(), ratios.end(), 1.0, minimun) > 0.9) {
        return new RegularPolygon(center, points_[0], 3);
    }
    // most close to 90° of each angle
    qreal const * amin = nullptr;
    amin = std::accumulate(angles.begin(), angles.end(), amin, [](auto min, qreal const & a) {
        return (min == nullptr || a < *min) ? &a : min;
    });
    int imin = amin - &angles.front();
    // maximun length ratio, length ratio of two approx equal edge
    qreal const * rmax = nullptr;
    rmax = std::accumulate(ratios.begin(), ratios.end(), rmax, [](auto max, qreal const & r) {
        return (max == nullptr || r > *max) ? &r : max;
    });
    int imax = rmax - &ratios.front();
    // IsoscelesTriangle
    if (*rmax > 0.9) {
        QPointF point = points_[imax];
        QPointF middle = (center * 3 - point) / 2; // botton middle
        qreal angle = GeometryHelper::angle(point - middle) - 90;
        QTransform tr; tr.rotate(angle);
        QPointF point2 = tr.map(point - middle) / 2;
        QPointF middle2 = -point2;
        qreal length = lengths[(imax + 2) % 3];
        if (imax == imin && *amin < 0.15) { // 8.6° IsoscelesRightTriangle
            length = point2.y() * 4;
        }
        middle2.setX(length / 2);
        center = (middle + point) / 2; //
        rotate_ = -angle;
        return new IsoscelesTriangle(point2 + center, middle2 + center);
    }
    // RightTriangle
    if (*amin < 0.15) { // 8.6°
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
        point2 = (pointa + pointb) / 2;
        return new RightTriangle(point2 - point3 / 2, point2 + point3 / 2);
    }
    return nullptr;
}

Geometry *ApproxGeometry::approxGeometry4()
{
    auto lengths = edgeLengths(points_);
    qDebug() << "approxGeometry4 length of each edges" << lengths;
    auto angles = edgeAngles(points_, lengths);
    qDebug() << "approxGeometry4 cos() of each angle" << QVector<qreal>(angles.begin(), angles.end());
    // angles of both side edges (cos())
    qreal a13 = GeometryHelper::dotProduct(points_[1] - points_[0], points_[3] - points_[2]) / lengths[1] / lengths[3];
    qreal a20 = GeometryHelper::dotProduct(points_[2] - points_[1], points_[0] - points_[3]) / lengths[2] / lengths[0];
    qDebug() << "approxGeometry4 angles of both side edges (cos())" << a13 << a20;
    if (a13 < -0.98 && a20 < -0.98) { // 11.5°
        // Parallelogram
        QPointF middle = (points_[0] + points_[2]) / 2;
        QPointF v01 = points_[1] - points_[0];
        qreal cos = GeometryHelper::dotProduct(v01, QPointF(1, 0)) / lengths[1];
        qreal sin = GeometryHelper::dotProduct(v01, QPointF(0, 1)) / lengths[1];
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
    } else if (a13 < -0.98 || a20 < -0.98) { // 11.5°
        // Trapezoid
        QPointF v = (a13 < -0.9) ? points_[1] - points_[0] : points_[2] - points_[1];
        qreal length = (a13 < -0.9) ? lengths[1] : lengths[2];
        qreal cos = GeometryHelper::dotProduct(v, QPointF(1, 0)) / length;
        qreal sin = GeometryHelper::dotProduct(v, QPointF(0, 1)) / length;
        QTransform tr(cos, -sin, sin, cos, 0, 0);
        rotate_ = -GeometryHelper::angle(v);
        qreal const * amin = nullptr;
        amin = std::accumulate(angles.begin(), angles.end(), amin, [](auto min, qreal const & a) {
            return (min == nullptr || a < *min) ? &a : min;
        });
        int idx = amin - &angles.front();
        int idx2 = (a13 < -0.9) ? 3 - idx : (idx ^ 1);
        // RightTrapezoid
        if (*amin < 0.1 && angles[idx2] < 0.1) { // 5.7°
            qDebug() << "approxGeometry4 right angle" << idx << idx2;
            qreal l13 = GeometryHelper::length(points_[2] - points_[0]);
            qreal l24 = GeometryHelper::length(points_[3] - points_[1]);
            QPointF middle = l13 < l24 ? (points_[3] + points_[1]) / 2 : (points_[2] + points_[0]) / 2;
            if (idx == 0 || idx2 == 0) {
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
        qreal ratio = (a13 < -0.9) ? smallRatio(lengths[0], lengths[2]) : smallRatio(lengths[1], lengths[3]);
        // IsopopeTrapezoid
        if (ratio > 0.9) {
            qDebug() << "approxGeometry4 edge length ratio" << ratio;
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

Geometry *ApproxGeometry::approxGeometryRegular()
{
    auto lengths = edgeLengths(points_);
    qDebug() << "approxGeometryRegular length of each edges" << lengths;
    auto angles = edgeAngles(points_, lengths);
    qDebug() << "approxGeometryRegular cos() of each angle" << angles;
    auto minmax = [](std::pair<qreal, qreal> m, qreal const & v) {
        if (v < m.first) m.first = v;
        if (v > m.second) m.second = v;
        return m;
    };
    std::pair<qreal, qreal> lminmax { 1E50, 0};
    lminmax = std::accumulate(lengths.begin(), lengths.end(), lminmax, minmax);
    std::pair<qreal, qreal> aminmax { 1E50, 0};
    aminmax = std::accumulate(angles.begin(), angles.end(), aminmax, minmax);
    qDebug() << "approxGeometryRegular" << lminmax << aminmax;
    qreal ratio = lminmax.first / lminmax.second;
    qreal angle = (acos(aminmax.first) - acos(aminmax.second)) * 180 / M_PI;
    qDebug() << "approxGeometryRegular" << ratio << angle;
    if (ratio > 0.7 && angle < 20) { // 20°
        // center point
        QPointF center = std::accumulate(points_.begin(), points_.end(), QPointF{0, 0}) / points_.size();
        QPointF l = points_.last();
        qreal c = 0;
        for (auto p : points_) {
            c += GeometryHelper::angle(l, center, p);
            l = p;
        }
        return new RegularPolygon(center, points_[0], points_.size(), static_cast<int>(c + 180) / 360);
    }
    return nullptr;
}


struct OBB
{
    QPointF u[2]; //x, y轴
    QPointF c;	//中心点
    QSizeF e;	//半长，半宽
};

float MinRatioRec(QVector<QPointF> const & pts, OBB &obb)
{
    float minRatio = 2;

    for(int i = 0, j = pts.size() - 1; i < pts.size(); j = i, i++) { //遍历边
        QPointF u0 = pts[i] - pts[j]; //构造边
        u0 = u0/ GeometryHelper::length(u0);
        QPointF u1 = QPointF(-u0.y(), u0.x()); //与u0垂直
        float min0 = 0.0f, max0 = 0.0f, min1 = 0.0f, max1 = 0.0f;

        for(int k = 0; k < pts.size(); k++) {//遍历点
            QPointF d = pts[k] - pts[j];
            //投影在u0
            float dot = GeometryHelper::dotProduct(d, u0);
            if (dot < min0) min0 = dot;
            if (dot > max0) max0 = dot;
            //投影在u1
            dot = GeometryHelper::dotProduct(d, u1);
            if (dot < min1) min1 = dot;
            if (dot > max1) max1 = dot;
        }

        float ratio = smallRatio(max0 - min0, max1 - min1);
        if( ratio < minRatio ) {
            minRatio = ratio;
            obb.c = pts[j] + ( u0 * (max0 + min0) + u1 * (max1 + min1) )*0.5f;

            obb.u[0] = u0;
            obb.u[1] = u1;

            obb.e.setWidth((max0 - min0) * 0.5f);
            obb.e.setHeight( (max1 - min1) * 0.5f);
        }
    }
    return minRatio;
}

static bool isConvexPolygon(QVector<QPointF> const & points)
{
    QPointF line = points.first() - points.last();
    qreal l = 0;
    for (int i = 0; i < points.size(); ++i) {
        int j = (i + 1 == points.size()) ? 0 : i + 1;
        qreal d = GeometryHelper::dotProduct({line.y(), -line.x()}, points[j] - points[i]);
        if (l * d < 0)
            return false;
        line = points[j] - points[i];
        l = d;
    }
    return true;
}

Geometry *ApproxGeometry::approxGeometryEllipse()
{
    if (isConvexPolygon(points_)) {
        OBB obb;
        qreal minRatio = MinRatioRec(points_, obb);
        qDebug() << "approxGeometryEllipse" << minRatio << obb.u[0] << obb.c << obb.e;
        if (minRatio > 0.8) {
            return new Circle(obb.c, (obb.e.width() + obb.e.height()) / 2);
        } else {
            rotate_ = -GeometryHelper::angle(obb.u[0]);
            return new Ellipse(obb.c, obb.e);
        }
    }
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

// result[0] = length(points[0] - points.last())
QVector<qreal> ApproxGeometry::edgeLengths(const QVector<QPointF> &points)
{
    QVector<qreal> result;
    QPointF s = points.last();
    for (auto p : points) {
        result.append(GeometryHelper::length(p - s));
        s = p;
    }
    return result;
}

// result[0] = angle(points[0] - points.last(), points[1] - points[0])
QVector<qreal> ApproxGeometry::edgeAngles(const QVector<QPointF> &points, QVector<qreal> const & lengths)
{
    QVector<qreal> result;
    QPointF line = points.first() - points.last();
    for (int i = 0; i < points.size(); ++i) {
        int j = (i + 1 == points.size()) ? 0 : i + 1;
        QPointF line2 = points[j] - points[i];
        result.append(qAbs(GeometryHelper::dotProduct(line2, line)) / lengths[j] / lengths[i]);
        line = line2;
    }
    return result;
}
