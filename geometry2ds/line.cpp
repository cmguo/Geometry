#include "line.h"
#include "geometryhelper.h"

#include <core/optiontoolbuttons.h>
#include <core/resource.h>
#include <core/toolbutton.h>

#include <QVector>
#include <QPainter>
#include <QMetaEnum>
#include <QGraphicsItem>

static char const * endtypes[] = {
    "", "o", "so",
    "a", "sa", "ha", "ra",
    "d", "sd",
    "b", "sb"
};

Line::Line(Resource * res)
    : Geometry2D(res, DrawAttach)
    , lineType_(Solid)
    , beginType_(None)
    , endType_(None)
{
    QByteArray type = res->property(Resource::PROP_SUB_TYPE).toByteArray();
    QByteArray endtype = res->property(Resource::PROP_SUB_TYPE2).toByteArray();
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
        char * p = endtype.data();
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
#ifdef QT_DEBUG
    setToolsString("lineType|线形|Popup,OptionsGroup,NeedUpdate|;"
                   "beginType|起点|Popup,OptionsGroup,NeedUpdate|;"
                   "endType|终点|Popup,OptionsGroup,NeedUpdate|;");
#endif
}

Line::Line(Line const & o)
    : Geometry2D(o)
    , lineType_(o.lineType_)
    , beginType_(o.beginType_)
    , endType_(o.endType_)
{
}

bool Line::moveElememt(int elem, const QPointF &pt)
{
    QPointF p(pt);
    GeometryHelper::attachToLines(points_[1 - elem], p);
    return Geometry2D::moveElememt(elem, p);
}

static QVector<qreal> penStyles[] = {
    {},
    {4, 2}, // Dash
    {1, 2}, // Dot
    {4, 2, 1, 2}, // DashDot
    {4, 2, 1, 2, 1, 2}, // DashDotDot
};

QPainterPath Line::visualPath()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPointF pt1 = points_[0];
    QPointF pt2 = points_[1];
    QPainterPathStroker ps;
    ps.setCapStyle(Qt::FlatCap);
    ps.setWidth(width_);
    if (beginType_ != None) {
        QPainterPath ph2;
        bool solid = false;
        fillEndian(ph2, beginType_, width_, pt1, pt1 - pt2, solid);
        ph |= solid ? ph2 : ps.createStroke(ph2);
    }
    if (endType_ != None) {
        QPainterPath ph2;
        bool solid = false;
        fillEndian(ph2, endType_, width_, pt2, pt2 - pt1, solid);
        ph |= solid ? ph2 : ps.createStroke(ph2);
    }
    QPainterPath ph2;
    ph2.moveTo(pt1);
    ph2.lineTo(pt2);
    ps.setDashPattern(penStyles[lineType_]);
    ph |= ps.createStroke(ph2);
    return ph;
}

QPainterPath Line::contour()
{
    QPainterPath ph;
    if (points_.size() < 2)
        return ph;
    QPointF pt1 = points_[0];
    QPointF pt2 = points_[1];
    bool solid = true;
    fillEndian(ph, beginType_, width_, pt1, pt1 - pt2, solid);
    fillEndian(ph, endType_, width_, pt2, pt2 - pt1, solid);
    QPointF dir(pt2 - pt1);
    QPointF udir = dir / GeometryHelper::length(dir);
    udir = QPointF(-udir.y(), udir.x()) * (GeometryHelper::HIT_DIFF + width_) * 0.5;
    ph.moveTo(pt1 - udir);
    ph.lineTo(pt2 - udir);
    ph.lineTo(pt2 + udir);
    ph.lineTo(pt1 + udir);
    ph.closeSubpath();
    return ph;
}

void Line::fillEndian(QPainterPath &ph, EndianType type, qreal width,
                      QPointF &pt, const QPointF &dir, bool & solid)
{
    QPointF udir = dir / GeometryHelper::length(dir);
    QRectF rect(-4, -4, 8, 8);
    rect.adjust(-width, -width, width, width);
    QTransform t(udir.x(), udir.y(), -udir.y(), udir.x(), pt.x(), pt.y());
    switch (type) {
    case None:
        break;
    case Ball:
    case SolidBall:
        if (solid) {
            pt = t.map(QPointF(rect.right(), 0));
            break;
        }
        ph.addEllipse(rect.adjusted(pt.x() + 1, pt.y() + 1, pt.x() - 1, pt.y() - 1));
        pt = t.map(QPointF(rect.left(), 0));
        solid = type == SolidBall;
        break;
    case Arrow:
        if (solid)
            break;
        ph.addPolygon(t.map(QPolygonF({
                                          QPointF(rect.left() * 2, rect.top()),
                                          QPointF(0, 0),
                                          QPointF(rect.left() * 2, rect.bottom())
                                      })));
        break;
    case SolidArrow:
    case HollowArrow:
        if (solid)
            break;
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
        if (solid)
            break;
        ph.addPolygon(t.map(QPolygonF({
                                          QPointF(rect.left() * 2, rect.top()),
                                          QPointF(0, 0),
                                          QPointF(rect.left() * 2, rect.bottom()),
                                          QPointF(rect.left(), 0),
                                          QPointF(rect.left() * 2, rect.top())
                                      })));
        pt = t.map(QPointF(rect.left(), 0));
        solid = true;
        break;
    case Diamond:
    case SolidDiamod:
        if (solid) {
            pt = t.map(QPointF(rect.right() * 2, 0));
            break;
        }
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
        if (solid) {
            pt = t.map(QPointF(rect.right(), 0));
            break;
        }
        ph.addPolygon(t.map(rect.adjusted(1, 1, -1, -1)));
        pt = t.map(QPointF(rect.left(), 0));
        solid = type == SolidBox;
        break;
    }
}

class Line::LineTypeToolButtons : public OptionToolButtons
{
public:
    LineTypeToolButtons()
        : OptionToolButtons(QList<LineType>{Solid, Dash, Dot, DashDot, DashDotDot}, 3)
    {
    }
protected:
    virtual QVariant buttonIcon(const QVariant &value) override
    {
        return QVariant::fromValue(lineIcon(value.value<LineType>()));
    }

private:
    static QGraphicsItem* lineIcon(LineType t)
    {
        QPainterPath ph;
        ph.addRect(QRectF(0, 0, 32, 32));
        QGraphicsPathItem * border = new QGraphicsPathItem(ph);
        border->setPen(Qt::NoPen);
        border->setBrush(QColor("#343434"));
        QGraphicsLineItem* item = new QGraphicsLineItem(border);
        item->setLine(QLine(4, 16, 28, 16));
        QPen pen(Qt::white, 2);
        pen.setCapStyle(Qt::FlatCap);
        pen.setDashPattern(penStyles[t]);
        item->setPen(pen);
        return border;
    }
};

class Line::EndianTypeToolButtons : public OptionToolButtons
{
public:
    EndianTypeToolButtons()
        : OptionToolButtons(QList<EndianType>{None, Ball, SolidBall, Arrow,
                            SolidArrow, HollowArrow, SharpArrow, Diamond,
                            SolidDiamod, Box, SolidBox}, 4)
    {
    }
protected:
    virtual QVariant buttonIcon(const QVariant &value) override
    {
        return QVariant::fromValue(endianIcon(value.value<EndianType>()));
    }

private:
    static QGraphicsItem* endianIcon(EndianType t)
    {
        QPainterPath ph;
        ph.addRect(QRectF(0, 0, 32, 32));
        QGraphicsPathItem * border = new QGraphicsPathItem(ph);
        border->setPen(Qt::NoPen);
        border->setBrush(QColor("#343434"));
        QPainterPath ph2;
        bool solid = false;
        QPointF pt1(16, 0);
        QPointF pt2(16, 16);
        switch (t) {
        case Arrow:
        case SolidArrow:
        case HollowArrow:
        case SharpArrow:
            pt1 += QPointF(6, 0);
            pt2 += QPointF(6, 0);
            break;
        default:
            break;
        }
        fillEndian(ph2, t, 2, pt2, pt1, solid);
        QGraphicsPathItem* item = new QGraphicsPathItem(border);
        item->setPath(ph2);
        item->setPen(QPen(Qt::white, 2));
        if (solid)
            item->setBrush(Qt::white);
        else
            item->setBrush(QBrush());
        return border;
    }
};

static Line::LineTypeToolButtons lineButtons;
static Line::EndianTypeToolButtons endianButtons;

REGISTER_OPTION_BUTTONS(Line, lineType, lineButtons)
REGISTER_OPTION_BUTTONS(Line, beginType, endianButtons)
REGISTER_OPTION_BUTTONS(Line, endType, endianButtons)
