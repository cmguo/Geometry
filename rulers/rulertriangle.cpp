#include "rulertriangle.h"
#include "core/resource.h"
#include "geometryhelper.h"

#include <QUrl>
#include <QDebug>

RulerTriangle::RulerTriangle(QVector<QPointF> const & stuns)
    : Polygon(new Resource("geometry2d", QUrl("geometry2d:polygon")))
    , stuns_(stuns)
{
}

void RulerTriangle::addPoint(const QPointF &pt)
{
    QPointF stuns[3][2] = {
        {stuns_[0], stuns_[1]},
        {stuns_[1], stuns_[2]},
        {stuns_[2], stuns_[0]},
    };
    for (int i = 0; i < 3; ++i) {
        QPointF dir = stuns[i][1] - stuns[i][0];
        GeometryHelper::adjustToLength(dir, width() / 2);
        dir = QPointF{dir.y(), -dir.x()};
        stuns[i][0] += dir;
        stuns[i][1] += dir;
    }
    for (int i = 0; i < 3; ++i) {
        int j = (i + 2) % 3;
        stuns_[i] = GeometryHelper::crossPoint(stuns[i][0], stuns[i][1], stuns[j][0], stuns[j][1]);
    }
    QPointF pt2 = attachToLine(pt);
    lastLine_ = curLine_;
    startLine_ = endLine_ = curLine_;
    Polygon::addPoint(pt2);
}

void RulerTriangle::movePoint(const QPointF &pt)
{
    QPointF pt2 = attachToLine(pt);
    // line: -2 ~ 0, [2] (1)
    // line: -1 ~ 0, (2)
    // line: -1 ~ 1, (2)
    // line: 0 ~ 0
    // line: 0 ~ 1
    // line: 0 ~ 2
    // line: 1 ~ 1
    // line: 1 ~ 2
    // line: 2 ~ 2
    if (curLine_ < 0)
        return;
    if (curLine_ > startLine_ && curLine_ < endLine_)
        return;
    if (curLine_ == 2 && startLine_ == -2 && endLine_ == 0)
        return;
    if (curLine_ == lastLine_) {
        int curLine = (curLine_ > endLine_) ? curLine_ - 3 : curLine_;
        if (points_.size() > 1) {
            QPointF s = startLine_ < curLine ? stuns_[curLine_] : points_.front();
            QPointF e = curLine < endLine_ ? stuns_[(curLine_ + 1) % 3] : points_.back();
            qreal d = GeometryHelper::dotProduct(pt2 - s, e - pt2);
            if (d < 0) {
                qreal d2 = GeometryHelper::dotProduct(pt2 - s, e - s);
                if (d2 < 0) {
                    if ((curLine_ - startLine_) % 3 == 0)
                        points_.front() = pt2;
                    else
                        return;
                } else {
                    if (curLine_ == endLine_)
                        points_.back() = pt2;
                    else
                        return;
                }
            } else {
                return;
            }
        } else {
            QPointF s = stuns_[curLine_];
            QPointF e = stuns_[(curLine_ + 1) % 3];
            qreal d2 = GeometryHelper::dotProduct(e - s, pt2 - points_.front());
            if (d2 < 0) {
                points_.prepend(pt2);
            } else {
                points_.append(pt2);
            }
        }
    } else if ((curLine_ - lastLine_ + 3) % 3 == 1) {
        points_.back() = stuns_[curLine_];
        points_.append(pt2);
        endLine_ = curLine_;
        if (endLine_ < startLine_) {
            startLine_ -= 3;
        }
    } else {
        points_.front() = stuns_[lastLine_];
        points_.prepend(pt2);
        startLine_ = curLine_;
        if (endLine_ < startLine_) {
            startLine_ -= 3;
        }
    }
    qDebug() << "RulerTriangle::movePoint" << startLine_ << endLine_ << points_;
    lastLine_ = curLine_;
    dirty_ = true;
}

bool RulerTriangle::commitPoint(const QPointF &pt)
{
    movePoint(pt);
    return true;
}

bool RulerTriangle::canClose()
{
    return false;
}

QPointF RulerTriangle::attachToLine(const QPointF &pt)
{
    QPointF pts[3];
    qreal len[3] = {0};
    for (int i = 0; i < 3; ++i) {
        QPointF d = stuns_[(i + 1) % 3] - stuns_[i];
        qreal dot1 = GeometryHelper::dotProduct(d, pt - stuns_[i]);
        qreal dot2 = GeometryHelper::length2(d);
        qreal r = dot1 / dot2;
        pts[i] = stuns_[i] + d * r;
        len[i] = GeometryHelper::length2(pts[i] - pt);
    }
    int l = len[0];
    curLine_ = 0;
    if (len[1] < l) {
        l = len[1];
        curLine_ = 1;
    }
    if (len[2] < l) {
        l = len[2];
        curLine_ = 2;
    }
    QPointF s = stuns_[curLine_];
    QPointF e = stuns_[(curLine_ + 1) % 3];
    QPointF p = pts[curLine_];
    if (GeometryHelper::dotProduct(p - s, e - p) < 0) {
        curLine_ = -1;
        return pt;
    }
    qDebug() << "RulerTriangle::attachToLine" << pt << curLine_ << pts[curLine_];
    return p;
}
