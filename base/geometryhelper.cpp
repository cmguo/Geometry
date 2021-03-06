#include "geometryhelper.h"
#include <algorithm/approxgeometry.h>

#include <core/resourcepage.h>

#include <QtMath>
#include <QApplication>
#include <QScreen>
#include <QFontMetrics>
#include <QDebug>
#include <QUrl>

#include <stdexcept>

qreal GeometryHelper::HIT_DIFF = 10.0;
qreal GeometryHelper::HIT_DIFF_DIFF = 100.0;
qreal GeometryHelper::DEFAULT_LINE_WIDTH = 4.0;
QFont GeometryHelper::TEXT_FONT;

static QFontMetrics textMetrics(GeometryHelper::TEXT_FONT);

static QColor defaultColor1;
static QColor defaultColor2;

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

void *GeometryHelper::approxGeometryBegin(qreal epsilon)
{
    return new ApproxGeometry{epsilon};
}

bool GeometryHelper::approxGeometryAddPoints(void *context, QVector<QPointF> points)
{
    ApproxGeometry * ag = reinterpret_cast<ApproxGeometry*>(context);
    return ag->addPoints(points);
}

QObject *GeometryHelper::approxGeometryFinish(void *context, QPointF const & offset)
{
    std::unique_ptr<ApproxGeometry> ag(reinterpret_cast<ApproxGeometry*>(context));
    return ag->finish(offset);
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

// ????????????
qreal GeometryHelper::length(QPointF const & vec)
{
    return sqrt(dotProduct(vec, vec));
}

// ?????????????????????
qreal GeometryHelper::length2(QPointF const & vec)
{
    return dotProduct(vec, vec);
}

// ?????????pt????????????(0,0)??????angle????????????angle??????????????????y?????????
void GeometryHelper::rotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() - pt.y() * angle.y(),
                 pt.x() * angle.y() + pt.y() * angle.x());
}

// ?????????pt????????????(0,0)??????angle????????????angle??????????????????y?????????
void GeometryHelper::reverseRotate(QPointF & pt, QPointF const & angle)
{
    pt = QPointF(pt.x() * angle.x() + pt.y() * angle.y(),
                 -pt.x() * angle.y() + pt.y() * angle.x());
}

qreal GeometryHelper::determinant(QPointF const & p1, QPointF const & p2)
{
    return p1.x() * p2.y() - p2.x() * p1.y();
}

// ?????????????????????
qreal GeometryHelper::dotProduct(const QPointF &p1, const QPointF &p2)
{
    return QPointF::dotProduct(p1, p2);
}

// ??????p?????????(p1~p2)????????????????????????????????????rp?????????
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

// ??????p??????(pt1~pt2)??????????????????????????????
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

// ????????????vec?????????????????????length?????????????????????
void GeometryHelper::adjustToLength(QPointF &vec, qreal length)
{
    qreal l = sqrt(length2(vec));
    if (qFuzzyIsNull(l))
        vec = vec + QPointF(length, 0);
    else
        vec = vec * (length / l);
}

// ?????????(start->end)?????????end?????????start???end??????????????????length
void GeometryHelper::adjustToLength(QPointF const & start, QPointF & end, qreal length)
{
    QPointF d = end - start;
    qreal l = sqrt(length2(d));
    if (qFuzzyIsNull(l))
        end = start + QPointF(length, 0);
    else
        end = start + d * (length / l);
}

// ?????????(p1~p2)?????????(q1~q2)?????????
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

static bool cmp(QPointF const &a, QPointF const & b) { // ??????????????????????????????????????????
    return GeometryHelper::determinant(a, b) > exps
            || (qAbs(GeometryHelper::determinant(a, b)) < exps
                && GeometryHelper::length(a) < GeometryHelper::length(b));
}

// ????????????pts???????????????????????????
QPolygonF GeometryHelper::smallestEnclosingPolygon(const QVector<QPointF> &pts)
{
    QPointF min = pts[0];
    for (QPointF const & pt : pts) {
        if (pt.y() < min.y() || (qFuzzyIsNull(pt.y() - min.y()) && pt.x() > min.x()))
            min = pt; // ????????????????????????????????????
    }
    QVector<QPointF> pts2(pts);
    for (QPointF & pt : pts2)
        pt -= min;
    std::sort(pts2.begin(), pts2.end(), cmp);
    QVector<QPointF> stack;
    stack.append(QPointF(0, 0)); // ??????
    for (QPointF & pt : pts2) { // ?????????
        while (stack.size() > 1) { //??????????????????
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
            stack.push_back(pt); //???????????????????????????????????????
    }
    stack.push_back(QPointF(0, 0));
    for (QPointF & pt : stack)
        pt += min;
    return QPolygonF(stack);
}

// ?????????p???????????????(p1~p2)????????????????????????????????????HIT_DIFF_DIFF???????????????
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

// ?????????p????????????pts???????????????????????????????????????????????????HIT_DIFF_DIFF???????????????
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
    QPointF(100, 100), QPointF(100, -100), // 45?? 135??
    QPointF(200, 0), QPointF(0, 200), // 0?? 90??
    QPointF(100, SQRT3W), QPointF(100, -SQRT3W), // 60?? 120??
    QPointF(SQRT3W, 100), QPointF(SQRT3W, -100) // 30?? 150??
};

void GeometryHelper::attachToLines(QPointF const & p1, QPointF & p)
{
    attachToLines(p1, ATTACH_DIRS, p);
}

void GeometryHelper::attachToLines(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    attachToLines(p1, p2, ATTACH_DIRS, p);
}

// ?????????p????????????p1??????????????????dirs??????????????????????????????????????????????????????HIT_DIFF_DIFF???????????????
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
