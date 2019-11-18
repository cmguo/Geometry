#include "line.h"

#include "core/resource.h"

#include <QVector>
#include <QPainter>

static char const * endtypes[] = {
    "", "o", "so",
    "a", "sa", "ha", "ra",
    "d", "sd",
    "b", "sb"
};

Line::Line(Resource * res)
    : Geometry2D(res)
    , lineType_(Solid)
    , beginType_(None)
    , endType_(None)
{
    QString type = res->property(Resource::PROP_SUB_TYPE).toString();
    QString endtype;
    int n = type.indexOf('.');
    if (n > 0) {
        endtype = type.mid(n + 1);
        type = type.left(n);
    }
    if (type == "solidline") {
        lineType_ = Solid;
    } else if (type == "dashline") {
        lineType_ = Dash;
    } else if (type == "dotline") {
        lineType_ = Dot;
    } else if (type == "dashdotline") {
        lineType_ = DashDot;
    } else if (type == "dashdotdotline") {
        lineType_ = DashDotDot;
    }
    if (!endtype.isEmpty()) {
        QChar * p = endtype.data();
        int n = sizeof (endtypes) / sizeof (endtypes[0]);
        if (*p != '-') {
            for (int i = 1; i < n; ++i) {
                if (p[0] == endtypes[i][0]
                        && (!endtypes[i][1] || p[1] == endtypes[i][1])) {
                    beginType_ = static_cast<EndianType>(i);
                    p += endtypes[i][1] ? 2 : 1;
                    break;
                }
            }
        }
        if (*p == '-') {
            ++p;
            for (int i = 1; i < n; ++i) {
                if (p[0] == endtypes[i][0]
                        && (!endtypes[i][1] || p[1] == endtypes[i][1])) {
                    endType_ = static_cast<EndianType>(i);
                    break;
                }
            }
        }
    }
}

Line::Line(QPointF const & pt)
    : Geometry2D(pt)
    , lineType_(Solid)
    , beginType_(None)
    , endType_(None)
{
}

Line::Line(Line const & o)
    : Geometry2D(o)
    , lineType_(o.lineType_)
    , beginType_(o.beginType_)
    , endType_(o.endType_)
{
}

QPainterPath Line::path()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPointF pt1 = points_[0];
    QPointF pt2 = points_[1];
    bool solid;
    fillEndian(ph, beginType_, width_, pt1, pt1 - pt2, solid);
    fillEndian(ph, endType_, width_, pt2, pt2 - pt1, solid);
    ph.moveTo(pt1);
    ph.lineTo(pt2);
    return ph;
}

void Line::draw(QPainter *painter)
{
    if (points_.size() < 2)
        return;
    QPointF pt1 = points_[0];
    QPointF pt2 = points_[1];
    painter->setPen(QPen(color_, width_));
    QPainterPath ph;
    bool solid = false;
    fillEndian(ph, beginType_, width_, pt1, pt1 - pt2, solid);
    if (solid)
        painter->setBrush(color_);
    else
        painter->setBrush(QBrush());
    painter->drawPath(ph);
    ph.clear();
    fillEndian(ph, endType_, width_, pt2, pt2 - pt1, solid);
    if (solid)
        painter->setBrush(color_);
    else
        painter->setBrush(QBrush());
    painter->drawPath(ph);
    QPen pen(color_, width_);
    switch (lineType_) {
    case Solid:
        break;
    case Dash:
        pen.setStyle(Qt::DashLine);
        break;
    case Dot:
        pen.setStyle(Qt::DotLine);
        break;
    case DashDot:
        pen.setStyle(Qt::DashDotLine);
        break;
    case DashDotDot:
        pen.setStyle(Qt::DashDotDotLine);
        break;
    }
    painter->setPen(pen);
    painter->drawLine(pt1, pt2);
}

void Line::fillEndian(QPainterPath &ph, EndianType type, qreal width,
                      QPointF &pt, const QPointF &dir, bool & solid)
{
    QPointF udir = dir / length(dir);
    QRectF rect(-4, -4, 8, 8);
    rect.adjust(-width, -width, width, width);
    QTransform t(udir.x(), udir.y(), -udir.y(), udir.x(), pt.x(), pt.y());
    switch (type) {
    case None:
        break;
    case Ball:
    case SolidBall:
        ph.addEllipse(rect.adjusted(pt.x() + 1, pt.y() + 1, pt.x() - 1, pt.y() - 1));
        pt = t.map(QPointF(rect.left(), 0));
        solid = type == SolidBall;
        break;
    case Arrow:
        ph.addPolygon(t.map(QPolygonF({
                                          QPointF(rect.left() * 2, rect.top()),
                                          QPointF(0, 0),
                                          QPointF(rect.left() * 2, rect.bottom())
                                      })));
        break;
    case SolidArrow:
    case HollowArrow:
        ph.addPolygon(t.map(QPolygonF({
                                          QPointF(rect.left() * 2, rect.top()),
                                          QPointF(0, 0),
                                          QPointF(rect.left() * 2, rect.bottom()),
                                          QPointF(rect.left() * 2, rect.top())
                                      })));
        pt = t.map(QPointF(rect.left() * 2, 0));
        solid = type == SolidArrow;
        break;
    case SharpArrow:
        ph.addPolygon(t.map(QPolygonF({
                                          QPointF(rect.left() * 2, rect.top()),
                                          QPointF(0, 0),
                                          QPointF(rect.left() * 2, rect.bottom()),
                                          QPointF(rect.left(), 0),
                                          QPointF(rect.left() * 2, rect.top())
                                      })));
        solid = true;
        break;
    case Diamond:
    case SolidDiamod:
        ph.addPolygon(t.map(QPolygonF({
                                          QPointF(0, rect.top()),
                                          QPointF(rect.right() * 2, 0),
                                          QPointF(0, rect.bottom()),
                                          QPointF(rect.left() * 2, 0),
                                          QPointF(0, rect.top())
                                      })));
        pt = t.map(QPointF(rect.left() * 2, 0));
        solid = type == SolidDiamod;
        break;
    case Box:
    case SolidBox:
        ph.addPolygon(t.map(rect.adjusted(1, 1, -1, -1)));
        pt = t.map(QPointF(rect.left(), 0));
        solid = type == SolidBox;
        break;
    }
}
