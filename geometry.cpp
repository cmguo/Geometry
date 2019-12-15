#include "geometry.h"
#include "geometryhelper.h"

#include <core/resource.h>
#include <core/toolbutton.h>

using namespace QtPromise;

Geometry::Geometry(Resource * res, Flags flags, Flags clearFlags)
    : ResourceView(res, flags | CanCopy, clearFlags)
    , dirty_(false)
    , color_(Qt::white)
    , width_(1.0)
{
}

Geometry::Geometry(QString const & type)
    : ResourceView(new Resource(type), CanCopy)
    , color_(Qt::white)
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

void Geometry::getToolButtons(QList<ToolButton *> & buttons,
                            ToolButton * parent)
{
    if (parent == nullptr) {
        QVariant ext = property("toolString");
        if (ext.isValid()) {
            static std::map<QMetaObject const *, QList<ToolButton *>> slist;
            auto iter = slist.find(res_->metaObject());
            if (iter == slist.end()) {
                std::map<QString, QList<ToolButton *>> t;
                iter = slist.insert(
                            std::make_pair(res_->metaObject(), ToolButton::makeButtons(ext.toString()))).first;
            }
            for (ToolButton * button : iter->second) {
                if (button->flags & ToolButton::NeedUpdate) {
                    updateToolButton(button);
                }
                buttons.insert(0,button);
            }

        }
    }
}


void Geometry::updateToolButton(ToolButton * button)
{
    (void) button;
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
    transform_.translate(transform_.scaleRotate().map(c));
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

