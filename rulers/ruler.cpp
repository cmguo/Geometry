#include "ruler.h"
#include "base/geometryhelper.h"

#include <core/resource.h>

#include <QPainter>

Ruler::Ruler(Resource * res, Flags flags, Flags clearFlags)
    : ResourceView(res, flags, clearFlags | CanCopy)
{
    width_ = height_ = 500;
}

QPointF Ruler::adjustDirection(QRectF &adjust)
{
    adjust.setRight(1);
    return QPointF(1, 0);
}

void Ruler::updateShape()
{
}

void Ruler::onDraw(QPainter *painter)
{
    (void) painter;
}

bool Ruler::hitTestTickMark(const QPointF &pos)
{
    return !shape2_.contains(pos);
}

QPointF Ruler::adjust(QPointF const & offset)
{
    QRectF adj;
    QPointF dir = adjustDirection(adj);
    qreal length = GeometryHelper::dotProduct(offset, dir);
    if (width_ + adj.width() * length < minWidth_)
        length = (minWidth_ - width_) / adj.width();
    QPointF topLeft = adj.topLeft() * length;
    QSizeF size = adj.size() * length;
    width_ += size.width();
    height_ += size.height();
    updateShape();
    return topLeft;
}

QVector<QPointF> Ruler::tickMarkPoints(QPointF const & from, QPointF const & to, int flags)
{
    qreal length = GeometryHelper::length(to - from);
    QPointF delta = to - from;
    GeometryHelper::adjustToLength(delta, Unit);
    QPointF scale = delta;
    if (flags & Anticlockwise)
        GeometryHelper::rotate(scale, QPointF(0, -1));
    else
        GeometryHelper::rotate(scale, QPointF(0, 1));
    QVector<QPointF> points;
    QPointF cur = from;
    while (length > 0) {
        int l = 0;
        if ((flags & CrossLittenEndian) && points.size() < 10) {
            l = points.size() / 2;
        } else if (points.size() % 20 == 0) { // per 10mm
            l = 4;
        } else if (points.size() % 10 == 0) { // pre 5mm
            l = 3;
        } else {
            l = 2;
        }
        QPointF end = cur + scale * l;
        if ((flags & ClipByShape) && l > 0 && !shape1_.contains(end) && !shape2_.contains(end))
            break;
        points.append(cur);
        points.append(end);
        length -= Unit;
        cur = cur + delta;
    }
    return points;
}

// Ticks with length 2, 3, 4 Units
void Ruler::drawTickMarks(QPainter *painter, const QPointF &from, const QPointF &to, int flags)
{
    if (flags & NeedRotate) {
        QPointF d = to - from;
        if (!qFuzzyIsNull(d.y())) {
            GeometryHelper::adjustToLength(d, 1.0);
            QTransform t = QTransform(d.x(), d.y(), -d.y(), d.x(), 0, 0)
                    * QTransform::fromTranslate(from.x(), from.y());
            QTransform t2 = t.inverted();
            QTransform o = painter->transform();
            painter->setTransform(t, true);
            if (flags & ClipByShape) {
                QPainterPath shape1 = shape1_;
                shape1_ = t2.map(shape1);
                QPainterPath shape2 = shape2_;
                shape2_ = t2.map(shape2);
                drawTickMarks(painter, {0, 0}, t2.map(to), flags & ~NeedRotate);
                shape2_ = shape2;
                shape1_ = shape1;
            } else {
                drawTickMarks(painter, {0, 0}, t2.map(to), flags & ~NeedRotate);
            }
            painter->setTransform(o);
            return;
        }
    }
    auto marks = tickMarkPoints(from, to, flags);
    painter->drawLines(marks);
    Qt::Alignment alignment = (flags & Anticlockwise)
            ? Qt::AlignBottom | Qt::AlignHCenter : Qt::AlignTop | Qt::AlignHCenter;
    for (int i = 0; i < marks.size(); i += 2) {
        if (i == 0 && (flags & CrossLittenEndian))
            continue;
        if (i % 20 == 0) {
            QString mark = QString::number(i / 20);
            QRectF txtRect = GeometryHelper::textRect(
                        mark, marks[i + 1], alignment);
            if (!shape2_.contains(txtRect))
                break;
            painter->drawText(txtRect, mark);
        }
    }
}

/*
 * RulerFactory
 */

RulerFactory::RulerFactory()
{
}

ResourceView *RulerFactory::create(Resource *res)
{
    QString type = res->url().path();
    int n = type.indexOf('/');
    if (n > 0)
        type = type.left(n);
    return ResourceFactory::create(res, type.toUtf8());
}

QUrl RulerFactory::newUrl(const QByteArray &type) const
{
    return QUrl("rulertool:" + type);
}

