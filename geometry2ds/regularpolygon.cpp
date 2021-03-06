#include "regularpolygon.h"
#include "base/geometryhelper.h"

#include <core/optiontoolbuttons.h>
#include <core/resource.h>
#include <core/toolbutton.h>

#include <QFile>
#include <QtMath>

RegularPolygon::RegularPolygon(Resource * res)
    : Polygon(res, DrawAttach)
    , nEdges_(0)
    , nSpan_(0)
{
    int edges = 3;
    int span = 1;
    QByteArray size(res->property(Resource::PROP_SUB_TYPE2).toByteArray());
    int n = size.indexOf('-');
    if (n > 0) {
        edges = size.left(n).toInt();
        span = size.mid(n + 1).toInt();
    } else if (!size.isEmpty()){
        edges = size.toInt();
    }
    setEdges(edges);
    setSpan(span);
}

RegularPolygon::RegularPolygon(RegularPolygon const & o)
    : Polygon(o)
    , nEdges_(0)
    , nSpan_(0)
{
    setEdges(o.nEdges_);
    setSpan(o.nSpan_);
}

RegularPolygon::RegularPolygon(QPointF &center, const QPointF &point, int edges, int span)
    : Polygon({center, point})
    , nEdges_(0)
    , nSpan_(0)
{
    setEdges(span == 1 ? edges : (edges << 8 | span));
}

class EdgeToolButtons : public OptionToolButtons
{
public:
    EdgeToolButtons()
#ifdef QT_DEBUG
        : OptionToolButtons({0x501, 0x601, 0x701, 0x801, 0x901, 0xa01,
                            0x502, 0x702, 0x703, 0x803, 0x902, 0x904}, 3)
#else
        : OptionToolButtons({0x501, 0x601}, 2)
#endif
    {
    }
protected:
    virtual QString buttonTitle(const QVariant &value) override
    {
        return buttonTitle(value.toInt());
    }
private:
    static QString buttonTitle(int n)
    {
        static QString numberChar;
        if (numberChar.isEmpty()) {
            QFile file(":/geometry/string/number.txt");
            file.open(QFile::ReadOnly);
            numberChar = file.readAll();
        }
        int e = n >> 8;
        int s = n & 0xff;
        return QString("正%1边形").arg(numberChar[e]) + (s == 1 ? "" : QString("%1").arg(s));
    }
};
static EdgeToolButtons edgeSpanButtons;
REGISTER_OPTION_BUTTONS(RegularPolygon, edgeSpan, edgeSpanButtons)

void RegularPolygon::setEdges(int n)
{
    int s = nSpan_;
    if (n > 256) {
        s = n & 0xff;
        n >>= 8;
    } else {
        s = 1;
    }
    if (nEdges_ == n && nSpan_ == s) {
        dirty_ = false;
        return;
    }
    nEdges_ = n;
    nSpan_ = 0;
    qreal radiusAttach = M_PI / 2 - M_PI / nEdges_;
    vAngleAttach_ = QPointF(cos(radiusAttach), sin(radiusAttach));
    setSpan(s);
}

void RegularPolygon::setSpan(int n)
{
    if (nSpan_ == n)
        return;
    nSpan_ = n;
    qreal radiusStep = M_PI * 2 * nSpan_ / nEdges_;
    vAngleStep_ = QPointF(cos(radiusStep), sin(radiusStep));
}

int RegularPolygon::edgeSpan()
{
    return nEdges_ << 8 | nSpan_;
}

int RegularPolygon::pointCount()
{
    return points_.size() == 2 ? nEdges_ : 1;
}

QPointF RegularPolygon::iterPoint(int index, QPointF &hint)
{
    QPointF center = points_.front();
    QPointF pt = points_[1];
    hint = pt - center;
    if (index == 0) {
        return pt;
    } else if (index == 1) {
        GeometryHelper::reverseRotate(hint, vAngleStep_);
        return center + hint;
    } else if (index == nEdges_ - 1) {
        GeometryHelper::reverseRotate(hint, QPointF(vAngleStep_.x(), -vAngleStep_.y()));
        return center + hint;
    } else {
        qreal radius = M_PI * 2 * nSpan_ * index / nEdges_;
        GeometryHelper::reverseRotate(hint, QPointF(cos(radius), sin(radius)));
        return center + hint;
    }
}

QPointF RegularPolygon::nextPoint(int index, QPointF &hint)
{
    (void) index;
    GeometryHelper::reverseRotate(hint, vAngleStep_);
    return points_.front() + hint;
}

QPointF RegularPolygon::prevPoint(int index, QPointF &hint)
{
    (void) index;
    GeometryHelper::reverseRotate(hint, QPointF(vAngleStep_.x(), -vAngleStep_.y()));
    return points_.front() + hint;
}

bool RegularPolygon::setPoint(int index, const QPointF &pt)
{
    (void) index;
    Polygon::setPoint(1, pt);
    return true;
}

bool RegularPolygon::moveElememt(int elem, const QPointF &pt)
{
    if (elem < nEdges_) {
        Polygon::setPoint(1, pt);
        QPointF vAngleAttach(-vAngleAttach_.x(), vAngleAttach_.y());
        GeometryHelper::attachToLines(points_[0], {vAngleAttach_, vAngleAttach}, points_[1]);
        return true;
    }
    elem -= nEdges_;
    QPointF hint;
    QPointF pt0 = points_[0];
    QPointF pt1 = iterPoint(elem, hint);
    QPointF pt2 = prevPoint(-1, hint);
    QPointF rp;
    qreal d = sqrt(GeometryHelper::dist2PointToSegment(pt1, pt2, pt0, rp));
    qreal dd = sqrt(GeometryHelper::dist2PointToSegment(pt1, pt2, pt, rp));
    pt1 = rp - pt0;
    pt2 = pt - pt0;
    if (GeometryHelper::length2(pt1) < GeometryHelper::length2(pt2))
        dd += d;
    else
        dd = d - dd;
    Polygon::setPoint(1, pt0 + (points_[1] - pt0) * dd / d);
    return true;
}
