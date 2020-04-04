#include "regularprismoid.h"
#include "geometryhelper.h"

#include <core/optiontoolbuttons.h>
#include <core/resource.h>
#include <core/toolbutton.h>

#include <QFile>
#include <QtMath>

RegularPrismoid::RegularPrismoid(Resource * res)
    : Polyhedron(res)
{
    setToolsString("edges|边数|Popup,OptionsGroup,NeedUpdate|;");

    int edges = 3;
    QByteArray size(res->property(Resource::PROP_SUB_TYPE2).toByteArray());
    if (!size.isEmpty())
        edges = size.toInt();
    setEdges(edges);
}

RegularPrismoid::RegularPrismoid(QPointF const & pt)
    : Polyhedron(pt)
{
}

RegularPrismoid::RegularPrismoid(RegularPrismoid const & o)
    : Polyhedron(o)
    , nEdges_(o.nEdges_)
    , ratio_(o.ratio_)
{
}

class EdgesToolButtons : public OptionToolButtons
{
public:
    EdgesToolButtons(QString const & title)
        : OptionToolButtons({3, 4, 5, 6, 7, 8}, 3)
        , title_(title)
    {
    }
protected:
    virtual QString buttonTitle(const QVariant &value) override
    {
        return buttonTitle(title_, value.toInt());
    }
private:
    static QString buttonTitle(QString const & title, int n)
    {
        static QString numberChar;
        if (numberChar.isEmpty()) {
            QFile file(":/geometry/string/number.txt");
            file.open(QFile::ReadOnly);
            numberChar = file.readAll();
        }
        return title.arg(numberChar[n]);
    }
private:
    QString title_;
};

static EdgesToolButtons edgesButtons("正%1棱柱");
REGISTER_OPTION_BUTTONS(RegularPrismoid, edges, edgesButtons)

void RegularPrismoid::setEdges(int n)
{
    nEdges_ = n;
    qreal radiusStep = M_PI * 2 / nEdges_;
    vAngleStep_ = QPointF(cos(radiusStep), sin(radiusStep));
    qreal radiusInit = M_PI / 2 - M_PI / nEdges_;
    if (nEdges_ > 4)
        radiusInit -= M_PI * 2 / nEdges_;
    vAngleInit_ = QPointF(cos(radiusInit), sin(radiusInit));
    clearLines();
    qreal ratio = r2(1.0);
    if (ratio <= 0) { // Orthoprism 正棱锥
        for (int i = 1; i <= nEdges_; ++i) {
            int j = i % nEdges_ + 1;
            makeLine(0, i);
            makeLine(i, j);
        }
    } else {
        for (int i = 0; i < nEdges_; ++i) {
            int j = (i + 1) % nEdges_;
            makeLine(i, j);
            makeLine(i, i + nEdges_);
            makeLine(i + nEdges_, j + nEdges_);
        }
    }
    emit changed();
}

int RegularPrismoid::pointCount()
{
    qreal ratio = r2(1.0);
    return points_.size() == 2 ? (ratio <= 0 ? 1 + nEdges_ : nEdges_ * 2) : 0;
}

/*
 * p1 -> (x0, y0, z0 + h)
 * p2 -> (x0 + r*cos(t), y0 - r*sin(t), z0)
 * y0 = 0
 * ==>
 * X1 = x0
 * Y1 = -z0 - h
 * X2 = x0 + cos(t)*r - C*sin(t)*r
 * Y2 = C*sin(t)*r - z0
 * ==>
 * x0 = X1
 * y0 = 0 (define)
 * z0 = C*r*sin(t) - Y2
 * h = -Y1 - z0
 * r = (X2 - X1) / (cos(t) - C*sin(t))
 */

QVector3D RegularPrismoid::point(int index)
{
    if (dirty_) {
        qreal x0 = points_[0].x();
        qreal y0 = 0;
        qreal r = (points_[1].x() - x0) / (vAngleInit_.x() - CO * vAngleInit_.y());
        qreal z0 = -points_[1].y() + CO * r * vAngleInit_.y();
        qreal h = -points_[0].y() - z0;
        origin_ = QVector3D(float(x0), float(y0), float(z0));
        size_ = QVector3D(float(r * vAngleInit_.x()), float(-r * vAngleInit_.y()), float(h));
        if (!finished())
            ratio_ = this->r2(r) / r;
    }
    QVector3D pt3(size_);
    if (ratio_ <= 0) {
        if (index == 0) {
            QVector3D pt = origin_;
            pt.setZ(size_.z());
            return pt;
        }
        --index;
        pt3.setZ(0);
    } else if (index < nEdges_) {
        pt3.setX(pt3.x() * float(ratio_));
        pt3.setY(pt3.y() * float(ratio_));
    } else {
        index -= nEdges_;
        pt3.setZ(0);
    }
    QPointF pt(pt3.toPoint());
    if (index == 1) {
        GeometryHelper::reverseRotate(pt, vAngleStep_);
    } else if (index == nEdges_ - 1) {
        GeometryHelper::rotate(pt, vAngleStep_);
    } else {
        qreal radius = M_PI * 2 * index / nEdges_;
        GeometryHelper::reverseRotate(pt, QPointF(cos(radius), sin(radius)));
    }
    pt3.setX(float(pt.x()));
    pt3.setY(float(pt.y()));
    return origin_ + pt3;
}

QVector<QPointF> RegularPrismoid::movePoints()
{
    QVector<QPointF> mpts = Polyhedron::movePoints();
    if (ratio_ > 0)
        mpts.append(points_[0]);
    mpts.append(points_[0] + QPointF(0, qreal(size_.z())));
    return mpts;
}

qreal RegularPrismoid::r2(qreal r)
{
    return r / 2.0;
}

bool RegularPrismoid::move(int elem, const QPointF &pt)
{
    QPointF pt2 = pt;
    if (elem < nEdges_ * 2) {
        qreal x1 = pt.x() - points_[0].x(); // x + Cy
        qreal y1 = pt.y() - points_[0].y(); // -Cy
        if (elem >= nEdges_)
            y1 -= qreal(size_.z());
        QPointF pt3(x1 + y1, -y1 / CO);
        if (elem > 0) {
            qreal radius = M_PI * 2 * elem / nEdges_;
            GeometryHelper::rotate(pt3, QPointF(cos(radius), sin(radius))); // reverse
            QVector3D ptm = PO.map(QVector3D(pt3.x(), pt3.y(), 0) + origin_);
            pt2 = QPointF(ptm.x(), ptm.y());
        }
        vAngleInit_ = QPointF(pt3.x(), -pt3.y());
        GeometryHelper::adjustToLength(vAngleInit_, 1);
        size_.setX(pt3.x());
        size_.setY(pt3.y());
        elem = 1;
    } else if (elem == nEdges_ * 2) {
        size_.setZ(-pt.y() - origin_.z());
        pt2.setX(points_[0].x());
        elem = 0;
    } else {
        qreal d = pt.y() + origin_.z();
        origin_.setZ(-pt.y());
        size_.setZ(size_.z() + d);
        pt2.setX(points_[1].x());
        pt2.setY(points_[1].y() + d);
        elem = 1;
    }
    Polyhedron::move(elem, pt2);
    dirty_ = false;
    return true;
}
