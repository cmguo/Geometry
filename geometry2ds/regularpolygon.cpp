#include "regularpolygon.h"

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
    QString size(res->property(Resource::PROP_SUB_TYPE2).toString());
    int n = size.indexOf('-');
    if (n > 0) {
        edges = size.left(n).toInt();
        span = size.mid(n + 1).toInt();
    } else if (!size.isEmpty()){
        edges = size.toInt();
    }
    setEdges(edges);
    setSpan(span);
    setProperty("toolString", "edges|边数|Popup,OptionsGroup,NeedUpdate|;");
}

RegularPolygon::RegularPolygon(QPointF const & pt)
    : Polygon(pt)
{
}

RegularPolygon::RegularPolygon(RegularPolygon const & o)
    : Polygon(o)
    , nEdges_(0)
    , nSpan_(0)
{
    setEdges(o.nEdges_);
    setSpan(o.nSpan_);
    setProperty("toolString", "edges|边数|Popup,OptionsGroup,NeedUpdate|;");
}

static QString buttonTitle(int n)
{
    static QString numberChar;
    if (numberChar.isEmpty()) {
        QFile file(":/geometry/strings/number.txt");
        file.open(QFile::ReadOnly);
        numberChar = file.readAll();
    }
    return QString("正%1边形").arg(numberChar[n]);
}

void RegularPolygon::getToolButtons(QList<ToolButton *> & buttons,
                            ToolButton * parent)
{
    if (parent == nullptr)
        return Polygon::getToolButtons(buttons, parent);
    if (parent->name == "edges") {
        for (int n : {5, 6}) {
            ToolButton::Flags flags;
            if (n == nEdges_)
                flags |= ToolButton::Selected;
            buttons.append(new ToolButton{
                               QVariant(n).toString(),
                               buttonTitle(n),
                               flags,
                               QVariant()
                           });
        }
    }
}


void RegularPolygon::updateToolButton(ToolButton * button)
{
    if (button->name == "edges") {
        button->title = buttonTitle(nEdges_);
    }
}

void RegularPolygon::setEdges(int n)
{
    if (nEdges_ == n)
        return;
    nEdges_ = n;
    nSpan_ = 0;
    qreal radiusAttach = M_PI / 2 - M_PI / nEdges_;
    vAngleAttach_ = QPointF(cos(radiusAttach), sin(radiusAttach));
    setSpan(1);
}

void RegularPolygon::setSpan(int n)
{
    if (nSpan_ == n)
        return;
    nSpan_ = n;
    qreal radius = M_PI * 2 * nSpan_ / nEdges_;
    vAngle_ = QPointF(cos(radius), sin(radius));
    emit changed();
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
        reverseRotate(hint, vAngle_);
        return center + hint;
    } else if (index == nEdges_ - 1) {
        reverseRotate(hint, QPointF(vAngle_.x(), -vAngle_.y()));
        return center + hint;
    } else {
        qreal radius = M_PI * 2 * nSpan_ * index / nEdges_;
        reverseRotate(hint, QPointF(cos(radius), sin(radius)));
        return center + hint;
    }
}

QPointF RegularPolygon::nextPoint(int index, QPointF &hint)
{
    (void) index;
    reverseRotate(hint, vAngle_);
    return points_.front() + hint;
}

QPointF RegularPolygon::prevPoint(int index, QPointF &hint)
{
    (void) index;
    reverseRotate(hint, QPointF(vAngle_.x(), -vAngle_.y()));
    return points_.front() + hint;
}

bool RegularPolygon::setPoint(int index, const QPointF &pt)
{
    (void) index;
    points_[1] = pt;
    return true;
}

bool RegularPolygon::move(int elem, const QPointF &pt)
{
    if (elem < nEdges_) {
        points_[1] = pt;
        QPointF vAngleAttach(-vAngleAttach_.x(), vAngleAttach_.y());
        attachToLines(points_[0], {vAngleAttach_, vAngleAttach}, points_[1]);
        return true;
    }
    elem -= nEdges_;
    QPointF hint;
    QPointF pt0 = points_[0];
    QPointF pt1 = iterPoint(elem, hint);
    QPointF pt2 = prevPoint(-1, hint);
    QPointF rp;
    qreal d = sqrt(dist2PointToSegment(pt1, pt2, pt0, rp));
    qreal dd = sqrt(dist2PointToSegment(pt1, pt2, pt, rp));
    pt1 = rp - pt0;
    pt2 = pt - pt0;
    if (QPointF::dotProduct(pt1, pt1) < QPointF::dotProduct(pt2, pt2))
        dd += d;
    else
        dd = d - dd;
    points_[1] = pt0 + (points_[1] - pt0) * dd / d;
    return true;
}
