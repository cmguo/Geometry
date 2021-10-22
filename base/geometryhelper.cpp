#include "geometryhelper.h"

#include <core/resource.h>
#include <core/resourcepage.h>
#include <core/resourcetransform.h>

#include <QtMath>
#include <QApplication>
#include <QScreen>
#include <QFontMetrics>
#include <QDebug>
#include <QUrl>

#include <stdexcept>

#include <geometry2ds/isoscelestriangle.h>
#include <geometry2ds/polygon.h>
#include <geometry2ds/regularpolygon.h>
#include <geometry2ds/righttriangle.h>

qreal GeometryHelper::HIT_DIFF = 10.0;
qreal GeometryHelper::HIT_DIFF_DIFF = 100.0;
qreal GeometryHelper::DEFAULT_LINE_WIDTH = 4.0;
QFont GeometryHelper::TEXT_FONT;

static QFontMetrics textMetrics(GeometryHelper::TEXT_FONT);

static QColor defaultColor1;
static QColor defaultColor2;

void RamerDouglasPeucker(const QVector<QPointF> &pointList, qreal epsilon, QVector<QPointF> &out);

GeometryHelper::GeometryHelper()
{
    init();
}

QColor GeometryHelper::defaultColor(ResourcePage * page)
{
    while (page->isSubPage())
        page = qobject_cast<ResourcePage*>(page->parent());
    return page->isIndependentPage() ? defaultColor2 : defaultColor1;
}

void GeometryHelper::setDefaultColor(ResourcePage *page, QColor color)
{
    while (page->isSubPage())
        page = qobject_cast<ResourcePage*>(page->parent());
    if (page->isIndependentPage())
        defaultColor2 = color;
    else
        defaultColor1 = color;
}

void GeometryHelper::init()
{
    static bool inited = false;
    if (inited)
        return;
    inited = true;
    qMetaTypeId<GeometryHelper*>();
    TEXT_FONT = QFont("Microsoft YaHei", 12);
    QSize sz  = QApplication::primaryScreen()->size();
    if (sz.width() > 1920) {
        HIT_DIFF = 20.0;
        HIT_DIFF_DIFF = 400.0;
        DEFAULT_LINE_WIDTH = 4.0;
        TEXT_FONT.setPixelSize(24);
    }
    textMetrics = QFontMetrics(TEXT_FONT);
    defaultColor1 = Qt::white;
    defaultColor2 = Qt::red;
}

QPointF GeometryHelper::textOffset(const QString &text, Qt::Alignment alignment)
{
    QRectF rect = textMetrics.boundingRect(text);
    QPointF off;
    if (alignment & Qt::AlignLeft)
        off.setX(rect.left());
    else if (alignment & Qt::AlignHCenter)
        off.setX(rect.left() + rect.width() / 2);
    else
        off.setX(rect.right());
    if (alignment & Qt::AlignTop)
        off.setY(rect.top());
    else if (alignment & Qt::AlignVCenter)
        off.setY(rect.top() + rect.height() / 2);
    else
        off.setY(rect.bottom());
    return -off;
}

QRectF GeometryHelper::textRect(const QString &text, const QPointF &alignTo, Qt::Alignment alignment)
{
    QRectF rect = textMetrics.boundingRect(text);
    QPointF off;
    if (alignment & Qt::AlignLeft)
        off.setX(rect.left());
    else if (alignment & Qt::AlignHCenter)
        off.setX(rect.left() + rect.width() / 2);
    else
        off.setX(rect.right());
    if (alignment & Qt::AlignTop)
        off.setY(rect.top());
    else if (alignment & Qt::AlignVCenter)
        off.setY(rect.top() + rect.height() / 2);
    else
        off.setY(rect.bottom());
    rect.translate(alignTo - off);
    return rect;
}

QPainterPath GeometryHelper::toRoundPolygon(const QPolygonF &polygon, qreal radius)
{
    if (polygon.isClosed())
        return toRoundPolygon(polygon, QVector<qreal>(polygon.size() - 1, radius));
    QVector<qreal> radiuses(polygon.size(), radius);
    radiuses.front() = 0;
    radiuses.back() = 0;
    return  toRoundPolygon(polygon, radiuses);
}

QPainterPath GeometryHelper::toRoundPolygon(const QPolygonF &polygon, QVector<qreal> const & radiuses)
{
    QPainterPath path;
    for (int i = 0; i < polygon.size() - 1; ++i) {
        qreal radius = radiuses[i];
        if (qFuzzyIsNull(radius)) {
            if (i > 0)
                path.lineTo(polygon[i]);
            else
                path.moveTo(polygon[i]);
            continue;
        }
        QPointF l = polygon[i == 0 ? polygon.size() - 2 : i - 1];
        QPointF const & c = polygon[i];
        QPointF n = polygon[i + 1];
        qreal ll = length(l - c);
        qreal ln = length(n - c);
        if (ll < ln)
            l = (l - c) * ln / ll + c;
        else
            n = (n - c) * ll / ln + c;
        QPointF o = (l + n) / 2;
        qreal d = dist2PointToSegment(l, c, o, l);
        d = radius / sqrt(d);
        l = (l - c) * d + c;
        o = (o - c) * d + c;
        adjustToLength(c, n, length(l - c));
        qreal a1 = angle(l - o);
        qreal a2 = angle(n - o);
        if (a1 + 180.0 < a2)
            a1 += 360.0;
        else if (a2 + 180.0 < a1)
            a2 += 360.0;
        QRectF r(0, 0, radius * 2, radius * 2);
        r.moveCenter(o);
        if (i > 0)
            path.lineTo(l);
        else
            path.moveTo(l);
        path.arcTo(r, a1, a2 - a1);
    }
    if (polygon.isClosed())
        path.closeSubpath();
    else
        path.lineTo(polygon.back());
    return path;
}

struct ApproxGeometry
{
    qreal epsilon;
    QVector<QPointF> points;
    qreal rotate = 0;
};

void *GeometryHelper::approxGeometryBegin(qreal epsilon)
{
    return new ApproxGeometry{epsilon, {}};
}

bool GeometryHelper::approxGeometryAddPoints(void *context, QVector<QPointF> points)
{
    ApproxGeometry * ag = reinterpret_cast<ApproxGeometry*>(context);
    QVector<QPointF> result;
    RamerDouglasPeucker(points, ag->epsilon, result);
    if (ag->points.empty()) {
        ag->points.append(result);
    } else {
        if (length(ag->points.front() - result.front()) < ag->epsilon) {
            result.pop_front();
            for (auto & p : result)
                ag->points.prepend(p);
        } else if (length(ag->points.front() - result.back()) < ag->epsilon) {
            result.pop_back();
            result.append(ag->points);
            ag->points = result;
        } else if (length2(ag->points.back() - result.back()) < ag->epsilon) {
            result.pop_back();
            while (!result.empty()) {
                ag->points.append(result.back());
                result.removeLast();
            }
        } else if (length2(ag->points.back() - result.front()) < ag->epsilon) {
            result.pop_front();
            ag->points.append(result);
        } else {
            ag->points = result;
            return false;
        }
    }
    return true;
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

static Geometry * approxGeometry3(ApproxGeometry & ag)
{
    qreal l1 = GeometryHelper::length(ag.points[1] - ag.points[0]);
    qreal l2 = GeometryHelper::length(ag.points[2] - ag.points[1]);
    qreal l3 = GeometryHelper::length(ag.points[0] - ag.points[2]);
    qDebug() << "approxGeometry3" << l1 << l2 << l3;
    qreal e12 = smallRatio(l1, l2);
    qreal e23 = smallRatio(l2, l3);
    qreal e31 = smallRatio(l3, l1);
    qDebug() << "approxGeometry3" << e12 << e23 << e31;
    QPointF center = std::accumulate(ag.points.begin(), ag.points.end(), QPointF{0, 0}) / 3;
    if (e12 > 0.9 && e23 > 0.9 && e31 > 0.9) {
        return new RegularPolygon(center, ag.points[0], 3);
    }
    // RightTriangle
    qreal a12 = qAbs(GeometryHelper::dotProduct(ag.points[1] - ag.points[0], ag.points[2] - ag.points[1])) / l1 / l2;
    qreal a23 = qAbs(GeometryHelper::dotProduct(ag.points[2] - ag.points[1], ag.points[0] - ag.points[2])) / l2 / l3;
    qreal a31 = qAbs(GeometryHelper::dotProduct(ag.points[0] - ag.points[2], ag.points[1] - ag.points[0])) / l3 / l1;
    qDebug() << "approxGeometry3" << a12 << a23 << a31;
    qreal amin = std::min(a12, a23);
    int imin = a12 > a23 ? 2 : 1;
    if (a31 < amin) {
        imin = 0;
        amin = a31;
    }
    // IsoscelesTriangle
    qreal emax = std::max(e12, e23);
    int imax = e12 < e23 ? 2 : 1;
    if (e31 > emax) {
        imax = 0;
        emax = e31;
    }
    if (emax > 0.9) {
        QPointF point = ag.points[imax];
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
        ag.rotate = -angle;
        return new IsoscelesTriangle(point2 + center, middle2 + center);
    }
    if (amin < 0.15) { // 8.6°
        QPointF point = ag.points[imin];
        QPointF pointa = imin == 0 ? ag.points[1] : ag.points[0];
        QPointF pointb = imin == 2 ? ag.points[1] : ag.points[2];
        QPointF point2 = pointb - pointa; point2 = {point2.y(), -point2.x()};
        if (GeometryHelper::dotProduct(point2, point - pointa) > 0)
            std::swap(pointa, pointb);
        GeometryHelper::dist2PointToSegment(pointa, pointb, point, point2);
        qreal lengtha = GeometryHelper::length(point2 - pointa);
        qreal lengthb = GeometryHelper::length(point2 - pointb);
        QPointF point3{sqrt(lengthb * (lengtha + lengthb)), sqrt(lengtha * (lengtha + lengthb))};
        ag.rotate = GeometryHelper::angle(point3) - GeometryHelper::angle(pointb - pointa);
        qDebug() << "approxGeometry3" << pointa << pointb << point3;
        point2 = (pointa + pointb) / 2;
        return new RightTriangle(point2 - point3 / 2, point2 + point3 / 2);
    }
    return nullptr;
}

QObject *GeometryHelper::approxGeometryFinish(void *context, QPointF const & offset)
{
    std::unique_ptr<ApproxGeometry> ag(reinterpret_cast<ApproxGeometry*>(context));
    if (ag->points.size() < 3)
        return nullptr;
    if (length(ag->points.back() - ag->points.front()) < ag->epsilon) {
        ag->points.pop_back();
        for (auto & p : ag->points) {
            p += offset;
        }
        Geometry * geometry = nullptr;
        if (ag->points.size() == 3) {
            geometry = approxGeometry3(*ag);
        }
        if (geometry == nullptr)
            geometry = new Polygon(ag->points);
        geometry->sync();
        geometry->finish(geometry->contour().boundingRect().center());
        geometry->sync();
        if (ag->rotate != 0) {
            geometry->transform().rotate(ag->rotate);
        }
        return geometry;
    }
    return nullptr;
}

qreal GeometryHelper::angle(QPointF const & vec) // axis angle
{
    if (qFuzzyIsNull(vec.x()))
        return vec.y() > 0 ? 270.0 : 90.0;
    //if (qFuzzyIsNull(vec.y()))
    //    return vec.x() > 0 ? 0.0 : 180.0;
    qreal r = atan(-vec.y() / vec.x());
    if (vec.x() < 0)
        r += M_PI;
    if (r < 0)
        r += M_PI * 2.0;
    //qDebug() << vec << r * 180.0 / M_PI;
    return r * 180.0 / M_PI;
}

qreal GeometryHelper::angle(QPointF const & p1, QPointF const & p2, QPointF const & p3)
{
    qreal dot = dotProduct(p1 - p2, p3 - p2);
    qreal mod1 = dotProduct(p1 - p2, p1 - p2);
    qreal mod2 = dotProduct(p3 - p2, p3 - p2);
    return acos(dot / sqrt(mod1 * mod2)) * 180 / M_PI;
}

// 矢量长度
qreal GeometryHelper::length(QPointF const & vec)
{
    return sqrt(dotProduct(vec, vec));
}

// 矢量长度的平方
qreal GeometryHelper::length2(QPointF const & vec)
{
    return dotProduct(vec, vec);
}

// 旋转点pt（相对于(0,0)点）angle的度数，angle是单位矢量，y轴向下
void GeometryHelper::rotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() - pt.y() * angle.y(),
                 pt.x() * angle.y() + pt.y() * angle.x());
}

// 旋转点pt（相对于(0,0)点）angle的度数，angle是单位矢量，y轴向上
void GeometryHelper::reverseRotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() + pt.y() * angle.y(),
                 -pt.x() * angle.y() + pt.y() * angle.x());
}

qreal GeometryHelper::determinant(QPointF const & p1, QPointF const & p2)
{
    return p1.x() * p2.y() - p2.x() * p1.y();
}

// 两个矢量的点乘
qreal GeometryHelper::dotProduct(const QPointF &p1, const QPointF &p2)
{
    return QPointF::dotProduct(p1, p2);
}

// 求点p到线段(p1~p2)的最近点，可能是端点，在rp中返回
qreal GeometryHelper::dist2PointToSegment(QPointF const & p1, QPointF const & p2,
                                QPointF const & p, QPointF & rp)
{
    QPointF d = p2 - p1;
    qreal dot1 = dotProduct(d, p - p1);
    if (dot1 <= 0)
        return length2(d);
    qreal dot2 = length2(p1 - p2);
    if (dot2 <= dot1)
        return length2(p - p2);
    qreal r = dot1 / dot2;
    rp = p1 + d * r;
    return length2(p - rp);
}

// 求点p直线(pt1~pt2)的垂直平分线的最近点
QPointF GeometryHelper::nearestPointAtVerticalBisector(QPointF const & pt1, QPointF const & pt2,
                                   QPointF const & p)
{
    QPointF c = (pt1 + pt2) / 2;
    QPointF d = pt1 - pt2;
    d = QPointF(-d.y(), d.x());
    qreal dot1 = dotProduct(d, p - c);
    qreal dot2 = length2(d);
    if (qFuzzyIsNull(dot2))
        return p;
    qreal r = dot1 / dot2;
    return c + d * r;
}

// 调整矢量vec，是其长度变为length，方向保持不变
void GeometryHelper::adjustToLength(QPointF &vec, qreal length)
{
    qreal l = sqrt(length2(vec));
    if (qFuzzyIsNull(l))
        vec = vec + QPointF(length, 0);
    else
        vec = vec * (length / l);
}

// 沿方向(start->end)调整点end，使点start和end之间的距离为length
void GeometryHelper::adjustToLength(QPointF const & start, QPointF & end, qreal length)
{
    QPointF d = end - start;
    qreal l = sqrt(length2(d));
    if (qFuzzyIsNull(l))
        end = start + QPointF(length, 0);
    else
        end = start + d * (length / l);
}

// 求直线(p1~p2)和直线(q1~q2)的交点
QPointF GeometryHelper::crossPoint(QPointF const & p1, QPointF const & p2,
                          QPointF const & q1, QPointF const & q2)
{
    QPointF a(p1.y() - p2.y(), q1.y() - q2.y()); // a1, a2
    QPointF b(p2.x() - p1.x(), q2.x() - q1.x()); // b1, b2
    QPointF c(determinant(p1, p2), determinant(q1, q2));
    qreal d = determinant(a, b);
    if (qFuzzyIsNull(d)) {
        if (qFuzzyIsNull(a.x()) && qFuzzyIsNull(b.x()))
            return p1;
        if (qFuzzyIsNull(a.y()) && qFuzzyIsNull(b.y()))
            return q1;
        return (p1 + p2 + q1 + q2) / 4;
    }
    return QPointF(determinant(b, c), determinant(c, a)) / d;
}

static constexpr double exps = 1e-9;

static bool cmp(QPointF const &a, QPointF const & b) { // 极角排序，极角相等按原点距排
    return GeometryHelper::determinant(a, b) > exps
            || (qAbs(GeometryHelper::determinant(a, b)) < exps
                && GeometryHelper::length(a) < GeometryHelper::length(b));
}

// 求点集合pts的最小包围凸多边形
QPolygonF GeometryHelper::smallestEnclosingPolygon(const QVector<QPointF> &pts)
{
    QPointF min = pts[0];
    for (QPointF const & pt : pts) {
        if (pt.y() < min.y() || (qFuzzyIsNull(pt.y() - min.y()) && pt.x() > min.x()))
            min = pt; // 最下右点，满足左拐检验法
    }
    QVector<QPointF> pts2(pts);
    for (QPointF & pt : pts2)
        pt -= min;
    std::sort(pts2.begin(), pts2.end(), cmp);
    QVector<QPointF> stack;
    stack.append(QPointF(0, 0)); // 原点
    for (QPointF & pt : pts2) { // 筛选栈
        while (stack.size() > 1) { //保证满足左拐
            QPointF now = stack.back();
            stack.pop_back();
            QPointF pre = stack.back();
            stack.push_back(now);
            if (determinant(now - pre, pt - now) >= 0.0) {
                stack.push_back(pt);
                break;
            }
            stack.pop_back();
        }
        if (stack.size() == 1)
            stack.push_back(pt); //只剩原点，没有边，直接进栈
    }
    stack.push_back(QPointF(0, 0));
    for (QPointF & pt : stack)
        pt += min;
    return QPolygonF(stack);
}

// 调整点p到其在直线(p1~p2)上的投影，如果距离大于于HIT_DIFF_DIFF，则不调整
void GeometryHelper::attachToLine(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    QPointF d = p2 - p1;
    qreal dot1 = dotProduct(d, p - p1);
    qreal dot2 = length2(d);
    qreal r = dot1 / dot2;
    QPointF rp = p1 + d * r;
    if (length2(rp - p) < HIT_DIFF_DIFF)
        p = rp;
}

// 调整点p到点集合pts中与其最近的点，如果最近距离大于于HIT_DIFF_DIFF，则不调整
int GeometryHelper::attachToPoints(QVector<QPointF> const & pts, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    int result = -1;
    for (int i = 0; i < pts.size(); ++i) {
        QPointF const & pt = pts[i];
        qreal r = length2(pt - p);
        if (r < min) {
            result = i;
            min = r;
        }
    }
    if (result >= 0)
        p = pts[result];
    return result;
}

static const qreal SQRT3W = 173.20508075688772935274463415059;

static QVector<QPointF> ATTACH_DIRS = {
    QPointF(100, 100), QPointF(100, -100), // 45° 135°
    QPointF(200, 0), QPointF(0, 200), // 0° 90°
    QPointF(100, SQRT3W), QPointF(100, -SQRT3W), // 60° 120°
    QPointF(SQRT3W, 100), QPointF(SQRT3W, -100) // 30° 150°
};

void GeometryHelper::attachToLines(QPointF const & p1, QPointF & p)
{
    attachToLines(p1, ATTACH_DIRS, p);
}

void GeometryHelper::attachToLines(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    attachToLines(p1, p2, ATTACH_DIRS, p);
}

// 调整点p到通过点p1的在所有方向dirs的直线的最近投影，如果最小距离大于于HIT_DIFF_DIFF，则不调整
int GeometryHelper::attachToLines(QPointF const & p1, QVector<QPointF> const & dirs, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    QPointF result;
    int index = -1;
    QPointF d1 = p - p1;
    for (int i = 0; i < dirs.size(); ++i) {
        QPointF d = dirs[i];
        qreal dot1 = dotProduct(d, d1);
        qreal dot2 = length2(d);
        qreal r = dot1 / dot2;
        QPointF rp = p1 + d * r;
        r = length2(rp - p);
        if (r < min) {
            result = rp;
            min = r;
            index = i;
        }
    }
    if (min < HIT_DIFF_DIFF)
        p = result;
    return index;
}

int GeometryHelper::attachToLines(QPointF const & p1, QPointF const & p2, QVector<QPointF> const & dirs, QPointF & p)
{
    qreal l = length(p2 - p1);
    QPointF tr = (p2 - p1) / l;
    QPointF pt = p - p1;
    reverseRotate(pt, tr);
    pt += p1;
    //QPointF t1 = pt;
    int index = attachToLines(p1, dirs, pt);
    if (index < 0)
        return index;
    //qDebug() << "attachToLines" << p1 << p2;
    //qDebug() << "attachToLines" << t1 << pt;
    pt -= p1;
    rotate(pt, tr);
    pt += p1;
    //qDebug() << "attachToLines" << p << pt;
    p = pt;
    return index;
}

int GeometryHelper::attachLineToPointsAndLines(const QVector<QPointF> &pts, QVector<int> lines, QPointF &lp1, QPointF &lp2)
{
    (void) lines;
    (void) lp2;
    qreal min1 = HIT_DIFF_DIFF,  min2 = HIT_DIFF_DIFF;
    int result1 = -1,  result2 = -1;
    for (int i = 0; i < pts.size(); ++i) {
        QPointF const & pt = pts[i];
        qreal r = length2(pt - lp1);
        if (r < min1) {
            result1 = i;
            min1 = r;
        } else if (r < min2) {
            result2 = i;
            min2 = r;
        }
    }
    return result1;
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

void RamerDouglasPeucker(const QVector<QPointF> &pointList, qreal epsilon, QVector<QPointF> &out)
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
