#include "qrthoprism.h"

static EdgesToolButtons edgesButtons("正%1棱锥");
REGISTER_OPTION_BUTTONS(Qrthoprism, edges, edgesButtons)

Qrthoprism::Qrthoprism(Resource * res)
    : RegularPrismoid(res, 0)
{
}

Qrthoprism::Qrthoprism(Qrthoprism const & o)
    : RegularPrismoid(o)
{
}

int Qrthoprism::pointCount()
{
    return points_.size() == 2 ? 1 + nEdges_ : 0;
}

QVector3D Qrthoprism::point(int index)
{
    if (index == 0) {
        QVector3D pt = origin_;
        pt.setZ(pt.z() + size_.z());
        return pt;
    }
    return RegularPrismoid::point(index + nEdges_ - 1);
}

// @see RegularPrismoid::movePoints()
bool Qrthoprism::move(int elem, const QPointF &pt)
{
    if (elem == 0)
        elem = nEdges_ * 2;
    else if (elem <= nEdges_)
        elem = elem + nEdges_ - 1;
    else
        elem = nEdges_ * 2 + 1;
    return RegularPrismoid::move(elem, pt);
}

void Qrthoprism::makeLines()
{
    for (int i = 1; i <= nEdges_; ++i) {
        int j = i % nEdges_ + 1;
        makeLine(0, i);
        makeLine(i, j);
    }
}
