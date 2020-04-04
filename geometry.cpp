#include "geometry.h"
#include "geometryhelper.h"

#include <core/resource.h>
#include <core/resourcetransform.h>
#include <core/toolbutton.h>

using namespace QtPromise;

Geometry::Geometry(Resource * res, Flags flags, Flags clearFlags)
    : ResourceView(res, flags | CanCopy, clearFlags)
    , dirty_(false)
    , color_("#FFF0F0F0")
    , width_(GeometryHelper::DEFAULT_LINE_WIDTH)
{
}

Geometry::Geometry(Geometry const & o)
    : ResourceView(o)
    , points_(o.points_)
    , dirty_(o.dirty_)
    , color_(o.color_)
    , width_(o.width_)
{
}

QPromise<void> Geometry::load()
{
    if (!Geometry::empty())
        return QPromise<void>::resolve();
    QWeakPointer<int> life(this->life());
    return res_->getStream().then([this, life](QSharedPointer<QIODevice> s) {
        if (life.isNull())
            return;
        QDataStream ds(s.get());
        int n = 0;
        qreal x, y;
        ds >> n;
        while (n) {
            ds >> x >> y;
            points_.append(QPointF(x, y));
            ++n;
        }
    });
}

void Geometry::setOption(const QByteArray &key, QVariant value)
{
    ResourceView::setOption(key, value);
    emit changed();
}

bool Geometry::empty() const
{
    return points_.isEmpty();
}

bool Geometry::finished() const
{
    return flags_ & DrawFinised;
}

void Geometry::clear()
{
    points_.clear();
}

void Geometry::addPoint(const QPointF &pt)
{
    points_.append(pt);
    dirty_ = true;
}

void Geometry::movePoint(const QPointF &pt)
{
    if (points_.size() == 1) {
        points_.append(pt);
    } else {
        points_.back() = pt;
        if (flags_ & DrawAttach)
            move(points_.size() - 1, pt);
    }
    dirty_ = true;
}

bool Geometry::commitPoint(const QPointF & pt)
{
    (void) pt;
    if (points_.size() == 1) {
        movePoint(points_.first() + QPointF(80, 80));
    }
    return true;
}

bool Geometry::moveTempPoint(const QPointF &pt)
{
    (void) pt;
    return false;
}

bool Geometry::canFinish()
{
    return false;
}

void Geometry::finish(const QPointF &c)
{
    for (int i = 0; i < points_.size(); ++i)
    {
        QPointF & pt = points_[i];
        pt -= c;
    }
    transform_->translate(transform_->scaleRotate().map(c));
    flags_ |= DrawFinised;
    dirty_ = true;
}

void Geometry::scale(qreal scale)
{
    for (int i = 0; i < points_.size(); ++i)
    {
        QPointF & pt = points_[i];
        pt *= scale;
    }
    dirty_ = true;
}

QVector<QPointF> Geometry::movePoints()
{
    return points_;
}

int Geometry::hit(QPointF & pt)
{
    for (int i = 0; i < points_.size(); ++i) {
        QPointF d = pt - points_[i];
        if (GeometryHelper::length2(d) < GeometryHelper::HIT_DIFF_DIFF) {
            pt = points_[i];
            return i;
        }
    }
    return -1;
}

bool Geometry::move(int elem, const QPointF &pt)
{
    if (elem < points_.size()) {
        points_[elem] = pt;
        dirty_ = true;
        return true;
    }
    return false;
}

