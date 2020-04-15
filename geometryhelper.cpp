#include "geometryhelper.h"

#include <QtMath>
#include <QApplication>
#include <QScreen>
#include <QFontMetrics>
#include <QDebug>

qreal GeometryHelper::HIT_DIFF = 10.0;
qreal GeometryHelper::HIT_DIFF_DIFF = 100.0;
qreal GeometryHelper::DEFAULT_LINE_WIDTH = 2.0;
QFont GeometryHelper::TEXT_FONT;

static QFontMetrics textMetrics(GeometryHelper::TEXT_FONT);

void GeometryHelper::init()
{
    TEXT_FONT = QFont();
    QSize sz  = QApplication::primaryScreen()->size();
    if (sz.width() > 1920) {
        HIT_DIFF = 20.0;
        HIT_DIFF_DIFF = 400.0;
        DEFAULT_LINE_WIDTH = 4.0;
        TEXT_FONT.setPixelSize(24);
    }
    textMetrics = QFontMetrics(TEXT_FONT);
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
    else if (alignment & Qt::AlignHCenter)
        off.setY(rect.top() + rect.height() / 2);
    else
        off.setY(rect.bottom());
    return -off;
}

QPainterPath GeometryHelper::toRoundPolygon(const QPolygonF &polygon, qreal radius)
{
    QPainterPath path;
    for (int i = 0; i < polygon.size() - 1; ++i) {
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
    path.closeSubpath();
    return path;
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

