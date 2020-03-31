#include "regularprism.h"
#include "geometryhelper.h"

#include <core/optiontoolbuttons.h>
#include <core/resource.h>
#include <core/toolbutton.h>

#include <QFile>
#include <QtMath>

RegularPrism::RegularPrism(Resource * res)
    : Polyhedron(res)
{
    setToolsString("edges|边数|Popup,OptionsGroup,NeedUpdate|;");

    int edges = 3;
    QString size(res->property(Resource::PROP_SUB_TYPE2).toString());
    if (!size.isEmpty())
        edges = size.toInt();
    setEdges(edges);
}

RegularPrism::RegularPrism(QPointF const & pt)
    : Polyhedron(pt)
{
}

RegularPrism::RegularPrism(RegularPrism const & o)
    : Polyhedron(o)
    , nEdges_(0)
    , origin_(o.origin_)
    , xyh_(o.xyh_)
{
}

class EdgesToolButtons : public OptionToolButtons
{
public:
    EdgesToolButtons()
        : OptionToolButtons({3, 4, 5, 6, 7, 8}, 3)
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
        return QString("正%1棱柱").arg(numberChar[n]);
    }
};
static EdgesToolButtons edgesButtons;
REGISTER_OPTION_BUTTONS(RegularPrism, edges, edgesButtons)

void RegularPrism::setEdges(int n)
{
    nEdges_ = n;
    qreal radiusStep = M_PI * 2 / nEdges_;
    vAngleStep_ = QPointF(cos(radiusStep), sin(radiusStep));
    qreal radiusInit = M_PI / 2 - M_PI / nEdges_;
    if (nEdges_ > 4)
        radiusInit -= M_PI * 2 / nEdges_;
    vAngleInit_ = QPointF(cos(radiusInit), sin(radiusInit));
    clearLines();
    for (int i = 0; i < nEdges_ - 1; ++i) {
        makeLine(i, i + 1);
        makeLine(i, i + nEdges_);
        makeLine(i + nEdges_, i + nEdges_ + 1);
    }
    makeLine(nEdges_ - 1, 0);
    makeLine(nEdges_ - 1, nEdges_ + nEdges_ - 1);
    makeLine(nEdges_ + nEdges_ - 1, nEdges_);
    emit changed();
}

int RegularPrism::pointCount()
{
    return points_.size() == 2 ? nEdges_ * 2 : 0;
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

QVector3D RegularPrism::point(int index)
{
    if (dirty_) {
        qreal x0 = points_[0].x();
        qreal y0 = 0;
        qreal r = (points_[1].x() - x0) / (vAngleInit_.x() - CO * vAngleInit_.y());
        qreal z0 = -points_[1].y() + CO * r * vAngleInit_.y();
        qreal h = -points_[0].y() - z0;
        origin_ = QVector3D(x0, y0, z0);
        xyh_ = QVector3D(r * vAngleInit_.x(), -r * vAngleInit_.y(), h);
    }
    QVector3D pt3(xyh_);
    if (index >= nEdges_) {
        index -= nEdges_;
        pt3.setZ(0);
    }
    QPointF pt(pt3.x(), pt3.y());
    if (index == 1) {
        GeometryHelper::reverseRotate(pt, vAngleStep_);
    } else if (index == nEdges_ - 1) {
        GeometryHelper::rotate(pt, vAngleStep_);
    } else {
        qreal radius = M_PI * 2 * index / nEdges_;
        GeometryHelper::reverseRotate(pt, QPointF(cos(radius), sin(radius)));
    }
    pt3.setX(pt.x());
    pt3.setY(pt.y());
    return origin_ + pt3;
}

QVector<QPointF> RegularPrism::movePoints()
{
    return Polyhedron::movePoints() + QVector<QPointF>({points_[0], points_[0] + QPointF(0, xyh_.z())});
}

bool RegularPrism::move(int elem, const QPointF &pt)
{
    QPointF pt2 = pt;
    if (elem < nEdges_ * 2) {
        qreal x1 = pt.x() - points_[0].x(); // x + Cy
        qreal y1 = pt.y() - points_[0].y(); // -Cy
        if (elem >= nEdges_)
            y1 -= xyh_.z();
        QPointF pt3(x1 + y1, -y1 / CO);
        if (elem > 0) {
            qreal radius = M_PI * 2 * elem / nEdges_;
            GeometryHelper::rotate(pt3, QPointF(cos(radius), sin(radius))); // reverse
            QVector3D ptm = PO.map(QVector3D(pt3.x(), pt3.y(), 0) + origin_);
            pt2 = QPointF(ptm.x(), ptm.y());
        }
        vAngleInit_ = QPointF(pt3.x(), -pt3.y());
        GeometryHelper::adjustToLength(vAngleInit_, 1);
        xyh_.setX(pt3.x());
        xyh_.setY(pt3.y());
        elem = 1;
    } else if (elem == nEdges_ * 2) {
        xyh_.setZ(-pt.y() - origin_.z());
        pt2.setX(points_[0].x());
        elem = 0;
    } else {
        qreal d = pt.y() + origin_.z();
        origin_.setZ(-pt.y());
        xyh_.setZ(xyh_.z() + d);
        pt2.setX(points_[1].x());
        pt2.setY(points_[1].y() + d);
        elem = 1;
    }
    Polyhedron::move(elem, pt2);
    dirty_ = false;
    return true;
}
