#include "geometry.h"

#include <core/resource.h>
#include <core/toolbutton.h>

using namespace QtPromise;

Geometry::Geometry(Resource * res)
    : ResourceView(res, CanCopy)
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
            }
            buttons.append(iter->second);
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

qreal Geometry::length(QPointF const & vec)
{
    return sqrt(QPointF::dotProduct(vec, vec));
}

qreal Geometry::length2(QPointF const & vec)
{
    return QPointF::dotProduct(vec, vec);
}

void Geometry::attachToLine(QPointF const & p1, QPointF const & p2, QPointF & p)
{
    QPointF d = p2 - p1;
    qreal dot1 = QPointF::dotProduct(d, p - p1);
    qreal dot2 = QPointF::dotProduct(d, d);
    qreal r = dot1 / dot2;
    QPointF rp = p1 + d * r;
    if (length2(rp - p) < HIT_DIFF_DIFF)
        p = rp;
}

static const qreal SQRT3W = 173.20508075688772935274463415059;

void Geometry::attachToLines(QPointF const & p1, QPointF & p)
{
    attachToLines(p1, {QPointF(100, 100), QPointF(100, -100),
                       QPointF(100, SQRT3W), QPointF(100, -SQRT3W),
                       QPointF(SQRT3W, 100), QPointF(SQRT3W, -100)}, p);
}

int Geometry::attachToLines(QPointF const & p1, QVector<QPointF> const & dirs, QPointF & p)
{
    qreal min = HIT_DIFF_DIFF;
    QPointF result;
    int index = -1;
    for (int i = 0; i < dirs.size(); ++i) {
        QPointF d = dirs[i];
        qreal dot1 = QPointF::dotProduct(d, p - p1);
        qreal dot2 = QPointF::dotProduct(d, d);
        qreal r = dot1 / dot2;
        QPointF rp = p1 + d * r;
        r = length2(rp - p);
        qDebug() << r;
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

int Geometry::attachToPoints(QVector<QPointF> const & pts, QPointF & p)
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

