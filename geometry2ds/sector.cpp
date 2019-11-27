#include "sector.h"

#include <core/toolbutton.h>

#include <QtMath>

Sector::Sector(Resource * res)
    : Geometry2D(res, DrawAttach)
{
    setProperty("toolString", "angle|角度|Popup,OptionsGroup,NeedUpdate|;");
}

Sector::Sector(QPointF const & pt)
    : Geometry2D(pt)
{
}

Sector::Sector(Sector const & o)
    : Geometry2D(o)
{
}

static QString buttonTitle(qreal a)
{
    return QString("%1°").arg(qRound(a));
}

void Sector::getToolButtons(QList<ToolButton *> & buttons,
                            ToolButton * parent)
{
    if (parent == nullptr)
        return Geometry2D::getToolButtons(buttons, parent);
    if (parent->name == "angle") {
        for (qreal n : {30, 45, 60, 90, 120, 180, 270, 360}) {
            ToolButton::Flags flags;
            if (qFuzzyIsNull(n - angle_))
                flags |= ToolButton::Selected;
            buttons.append(new ToolButton{
                               QVariant(n).toString(),
                               buttonTitle(n),
                               flags,
                               QVariant()
                           });
        }
    }
}

void Sector::updateToolButton(ToolButton * button)
{
    if (button->name == "angle") {
        button->title = buttonTitle(angle());
    }
}

void Sector::setAngle(qreal angle)
{
    if (qFuzzyIsNull(angle - angle_))
        return;
    qreal la = angle;
    //if (la > 0 && la > angle_ + 180.0)
    //    la -= 360.0;
    //else if (la < 0 && la < angle_ - 180.0)
    //    la += 360.0;
    angle_ = la;
    la = la * M_PI / 180.0;
    QPointF pt3;
    qreal r = length(points_[1] - points_[0]);
    if (points_.size() > 2) {
        pt3 = points_[2];
    } else {
        pt3 = points_[0] + QPointF(r, 0);
    }
    QPointF d = pt3 - points_[0];
    reverseRotate(d, QPointF(cos(la), -sin(la)));
    points_[1] = points_[0] + d;
    emit changed();
}

QPainterPath Sector::path()
{
    QPainterPath ph;
    if (points_.size() > 1)
    {
        QPointF pt1 = points_[0];
        QPointF pt2 = points_[1];
        ph.moveTo(pt1);
        ph.lineTo(pt2);
        qreal r = length(pt2 - pt1);
        QPointF pt3;
        if (points_.size() > 2) {
            pt3 = points_[2];
        } else {
            pt3 = pt1 + QPointF(r, 0);
        }
        QRectF rect(-r, -r, r * 2, r * 2);
        rect.moveCenter(points_[0]);
        qreal a1 = Geometry2D::angle(pt2 - pt1);
        qreal a2 = Geometry2D::angle(pt3 - pt1);
        qreal la = a2 - a1;
        if (la > 0 && la > angle_ + 180.0)
            la -= 360.0;
        else if (la < 0 && la < angle_ - 180.0)
            la += 360.0;
        if (qFuzzyIsNull(la))
            la = qAbs(angle_) < 180 ? 0 : 360;
        angle_ = la;
        qDebug() << "sector" << a1 << a2;
        qDebug() << "sector angle" << (a2 - a1) << angle_;
        ph.arcTo(rect, a1, la);
        ph.closeSubpath();
        if (angle_ < 0)
            addAngleLabeling(ph, pt3, pt1, pt2, -angle_);
        else
            addAngleLabeling(ph, pt2, pt1, pt3, angle_);
    }
    return ph;
}

QVector<QPointF> Sector::movePoints()
{
    QVector<QPointF> points;
    points.append(points_);
    qreal r = length(points_[1] - points_[0]);
    QPointF pt3;
    if (points_.size() > 2) {
        pt3 = points_[2];
    } else {
        pt3 = points_[0] + QPointF(r, 0);
        points.append(pt3);
    }
    points.append((points_[0] + points_[1]) / 2);
    points.append((points_[0] + pt3) / 2);
    if (qFuzzyIsNull(angle() - 180)) {
        pt3 = points_[1] - points_[0];
        pt3 = points_[0] - QPointF(-pt3.y(), pt3.x());
        if (angle_ < 0)
            pt3 = points_[0] * 2 - pt3;
    } else {
        pt3 = (points_[1] + pt3) / 2;
        adjustToLength(points_[0], pt3, r);
        if (angle_ > 180 || angle_ < -180)
            pt3 = points_[0] * 2 - pt3;
    }
    points.append(pt3);
    return points;
}

int Sector::hit(QPointF &pt)
{
    int elem = Geometry2D::hit(pt);
    if (elem >= 0)
        return elem;
    QPointF pt1 = points_[0];
    QPointF pt2 = points_[1];
    QPointF pt3;
    QPointF d = pt2 - pt1;
    qreal r = sqrt(QPointF::dotProduct(d, d));
    if (points_.size() == 2) {
        pt3 = pt1 + QPointF(r, 0);
    } else {
        pt3 = points_[2];
    }
    d = pt3 - pt;
    if (QPointF::dotProduct(d, d) < HIT_DIFF_DIFF) {
        pt = pt3;
        return 2;
    }
    QPointF rp;
    if (dist2PointToSegment(pt1, pt2, pt, rp) < HIT_DIFF_DIFF) {
        pt = rp;
        return 3;
    }
    if (dist2PointToSegment(pt1, pt3, pt, rp) < HIT_DIFF_DIFF) {
        pt = rp;
        return 4;
    }
    QPointF dd = pt - pt1;
    qreal rr = sqrt(QPointF::dotProduct(dd, dd));
    if (rr + HIT_DIFF > r && rr - HIT_DIFF < r)
        return 5;
    return -1;
}

bool Sector::move(int elem, const QPointF &pt)
{
    if (elem == 0) {
        if (points_.size() == 2)
            points_[0] = pt;
        else
            points_[0] = nearestPointAtVerticalBisector(points_[1], points_[2], pt);
    } else if (elem == 1) {
        points_[1] = pt;
        QPointF pt3;
        if (points_.size() > 2) {
            pt3 = points_[2];
        } else {
            qreal r = length(points_[1] - points_[0]);
            pt3 = points_[0] + QPointF(r, 0);
        }
        attachToLines(points_[0], pt3, points_[1]);
        if (points_.size() == 3)
            adjustToLength(points_[0], points_[2], length(points_[0] - points_[1]));
    } else if (elem == 2) {
        if (points_.size() == 2)
            points_.append(pt);
        else
            points_[2] = pt;
        attachToLines(points_[0], points_[1], points_[2]);
        adjustToLength(points_[0], points_[1], length(points_[0] - points_[2]));
    } else if (elem == 3) {
        qreal r = length(points_[0] - points_[1]);
        points_[1] = pt;
        QPointF pt3;
        if (points_.size() > 2) {
            pt3 = points_[2];
        } else {
            pt3 = points_[0] + QPointF(r, 0);
        }
        attachToLines(points_[0], pt3, points_[1]);
        adjustToLength(points_[0], points_[1], r);
    } else if (elem == 4) {
        qreal r = length(points_[0] - points_[1]);
        if (points_.size() == 2)
            points_.append(pt);
        else
            points_[2] = pt;
        attachToLines(points_[0], points_[1], points_[2]);
        adjustToLength(points_[0], points_[2], r);
    } else if (elem == 5) {
        qreal r = length(points_[0] - pt);
        adjustToLength(points_[0], points_[1], r);
        if (points_.size() == 3)
            adjustToLength(points_[0], points_[2], r);
    }
    return true;
}
