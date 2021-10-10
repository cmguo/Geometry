#include "rulertriangle.h"
#include "base/geometryhelper.h"

#include <core/resource.h>

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
    if (canClose())
        return;
    QPointF pt2 = attachToLine(pt);
    if (curLine_ < 0)
        return;
    int diff = (curLine_ - lastLine_ + 4) % 3 - 1;
    curLine_ = lastLine_ + diff;
    if (curLine_ > startLine_ && curLine_ < endLine_)
        return;
    if (curLine_ + 3 < endLine_ || startLine_ + 3 < curLine_) {
        startLine_ = 0;
        endLine_ = -1;
        points_ = stuns_;
        dirty_ = true;
        return;
    }
    if (diff == 1) {
        qDebug() << "RulerTriangle::movePoint inc line" << curLine_;
        if (curLine_ > endLine_) {
            points_.back() = stuns_[(curLine_ + 3) % 3];
            points_.append(pt2);
            endLine_ = lastLine_ = curLine_;
            dirty_ = true;
            qDebug() << "RulerTriangle::movePoint line range" << startLine_ << endLine_;
            return;
        }
        lastLine_ = curLine_;
    } else if (diff == -1) {
        qDebug() << "RulerTriangle::movePoint dec line" << curLine_;
        if (curLine_ < startLine_) {
            points_.front() = stuns_[(lastLine_ + 3) % 3];
            points_.prepend(pt2);
            startLine_ = lastLine_ = curLine_;
            dirty_ = true;
            qDebug() << "RulerTriangle::movePoint line range" << startLine_ << endLine_;
            return;
        }
        lastLine_ = curLine_;
    }
    // qDebug() << "RulerTriangle::movePoint" << curLine_ << diff << pt;
    if (points_.size() > 1) {
        QPointF s = startLine_ < curLine_ ? stuns_[(curLine_ + 3) % 3] : points_.front();
        QPointF e = curLine_ < endLine_ ? stuns_[(curLine_ + 4) % 3] : points_.back();
        qreal d = GeometryHelper::dotProduct(pt2 - s, e - pt2);
        if (d < 0) {
            qreal d2 = GeometryHelper::dotProduct(pt2 - s, e - s);
            if (d2 < 0) {
                if (curLine_ == startLine_) {
                    points_.front() = pt2;
                } else {
                    return;
                }
            } else {
                if (curLine_ == endLine_) {
                    points_.back() = pt2;
                } else {
                    return;
                }
            }
            if (startLine_ + 3 == endLine_) {
                s = stuns_[(curLine_ + 3) % 3];
                e = stuns_[(curLine_ + 4) % 3];
                qreal d3 = GeometryHelper::dotProduct(points_.front() - points_.back(), e - s);
                if (d3 < 0) {
                    startLine_ = 0;
                    endLine_ = -1;
                    points_ = stuns_;
                }
            }
        } else {
            return;
        }
    } else {
        QPointF s = stuns_[(curLine_ + 3) % 3];
        QPointF e = stuns_[(curLine_ + 4) % 3];
        qreal d2 = GeometryHelper::dotProduct(e - s, pt2 - points_.front());
        if (d2 < 0) {
            points_.prepend(pt2);
        } else {
            points_.append(pt2);
        }
    }
    dirty_ = true;
    qDebug() << "RulerTriangle::movePoint" << startLine_ << curLine_ << endLine_ << points_;
}

bool RulerTriangle::commitPoint(const QPointF &pt)
{
    movePoint(pt);
    return true;
}

bool RulerTriangle::canClose()
{
    return endLine_ < startLine_;
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
//    qDebug() << "RulerTriangle::attachToLine" << pt << curLine_ << pts[curLine_];
    return p;
}
