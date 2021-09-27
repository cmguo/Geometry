#include "geometry.h"
#include "geometryhelper.h"

#include <core/resource.h>
#include <core/resourcepage.h>
#include <core/resourcerecord.h>
#include <core/resourcetransform.h>
#include <core/toolbutton.h>

#include <quick/quickhelper.h>

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
    auto life(this->life());
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

bool Geometry::setOption(const QByteArray &key, QVariant value)
{
    QVariant old = getOption(key);
    if (!ResourceView::setOption(key, value))
        return false;
    QVariant crt = getOption(key);
    if (old != crt) {
        RecordMergeScope rs(this);
        if (rs)
            rs.add(MakeFunctionRecord([this, key, old]() {
                setOption(key, old);
            }, [this, key, crt]() {
                setOption(key, crt);
            }));
        dirty_ = true;
        emit changed(key);
    }
    return true;
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
            moveElememt(points_.size() - 1, pt);
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

void Geometry::sync()
{
    dirty_ = false;
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
    // ignore scale transform
    transform_->translate(transform_->rotate().map(c));
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

void Geometry::setColor(QColor color)
{
    color_ = color;
    GeometryHelper::setDefaultColor(qobject_cast<ResourcePage*>(parent()), color);
}

void Geometry::setWidth(qreal width)
{
    width_ = width;
}

void Geometry::init()
{
    color_ = GeometryHelper::defaultColor(qobject_cast<ResourcePage*>(parent()));
}

QVector<QPointF> Geometry::movePoints()
{
    return points_;
}

QObject *Geometry::toQuickPath(QObject * context)
{
    QObject * shape = QuickHelper::createObject(context, "ShapePath", "QtQuick.Shapes");
    shape->setProperty("strokeColor", QColor(Qt::transparent));
    shape->setProperty("strokeWidth", 0);
    shape->setProperty("fillColor", color_);
    fillQuickPath(shape, visualPath());
    return shape;
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

void Geometry::beginMove(int)
{
    setProperty("editStartPoints", QVariant::fromValue(points_));
    setProperty("editStartOffset", transform().offset());
}

bool Geometry::moveElememt(int elem, const QPointF &pt)
{
    if (elem < points_.size()) {
        points_[elem] = pt;
        dirty_ = true;
        return true;
    }
    return false;
}

void Geometry::endMove(int)
{
    auto points = property("editStartPoints").value<QVector<QPointF>>();
    QPointF off = property("editStartOffset").value<QPointF>() - transform().offset();
    for (auto & p : points)
        p += off;
    auto points2 = points_;
    for (auto & p : points2)
        p -= off;
    if (!points.isEmpty()) {
        RecordMergeScope rs(this);
        if (rs)
            rs.add(MakeFunctionRecord([this, points]() {
                points_ = points; dirty_ = true; emit changed("shape");
            }, [this, points2]() {
                points_ = points2; dirty_ = true; emit changed("shape");
            }));
        setProperty("editStartPoints", QVariant());
    }
}

QPainterPath Geometry::graphPath()
{
    return QPainterPath();
}

QPainterPath Geometry::textPath()
{
    return QPainterPath();
}

QPainterPath Geometry::contour()
{
    QPainterPath ph = graphPath();
    QPainterPath ph2 = textPath();
    if (!ph2.isEmpty() || !ph.contains(ph2))
        ph |= ph2;
    return ph;
}

QPainterPath Geometry::visualPath()
{
    QPainterPathStroker ps;
    ps.setCapStyle(Qt::RoundCap);
    ps.setWidth(width_);
    QPainterPath ph = ps.createStroke(graphPath());
    ph |= textPath();
    return ph;
}

void Geometry::fillQuickPath(QObject *path, const QPainterPath &ph)
{
    QObject * seg = nullptr;
    bool ready = true;
    constexpr char const * classNames[] = {"PathMove", "PathLine", "PathCubic"};
    for (int i = 0; i < ph.elementCount(); ++i) {
        QPainterPath::Element e = ph.elementAt(i);
        switch (e.type) {
        case QPainterPath::MoveToElement:
        case QPainterPath::LineToElement:
        case QPainterPath::CurveToElement:
            seg = QuickHelper::createObject(path, classNames[e.type], "QtQuick", "2.12");
            seg->setProperty("x", e.x);
            seg->setProperty("y", e.y);
            seg->setProperty("step", 1);
            ready = e.type != QPainterPath::CurveToElement;
            break;
        case QPainterPath::CurveToDataElement:
            if (seg->property("step").isValid()) {
                seg->setProperty("control1X", e.x);
                seg->setProperty("control1Y", e.y);
                seg->setProperty("step", QVariant());
            } else {
                seg->setProperty("control2X", e.x);
                seg->setProperty("control2Y", e.y);
                ready = true;
            }
            break;
        }
        if (ready) {
            QuickHelper::appendChild(path, seg);
            seg = nullptr;
        }
    }
}

