#include "geometry.h"

#include <core/resource.h>

using namespace QtPromise;

Geometry::Geometry(Resource * res)
    : ResourceView(res, CanCopy)
    , dirty_(false)
    , color_(Qt::black)
    , width_(1.0)
{
}

Geometry::Geometry(QString const & type)
    : ResourceView(new Resource(type), CanCopy)
    , color_(Qt::black)
    , width_(1.0)
{
}

Geometry::Geometry(Geometry const & o)
    : ResourceView(o)
    , color_(o.color_)
    , width_(o.width_)
{
    points_ = o.points_;
}

QPromise<void> Geometry::load()
{
    if (!Geometry::empty())
        return QPromise<void>::resolve();
    QWeakPointer<int> life(this->life());
    return res_->getStream().then([this, life](QIODevice * s) {
        if (life.isNull())
            return;
        QDataStream ds(s);
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

bool Geometry::empty() const
{
    return points_.isEmpty();
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
    if (points_.size() > 1)
        points_.pop_back();
    points_.append(pt);
    dirty_ = true;
}

bool Geometry::commit(const QPointF & pt)
{
    (void) pt;
    if (points_.size() == 1) {
        movePoint(points_.first() + QPointF(80, 80));
    }
    return true;
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
    transform_.translate(c.x(), c.y());
    flags_ |= DrawFinised;
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
        if (QPointF::dotProduct(d, d) < HIT_DIFF_DIFF) {
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
