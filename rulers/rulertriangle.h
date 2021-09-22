#ifndef RULERTRIANGLE_H
#define RULERTRIANGLE_H

#include "geometry2ds/polygon.h"

class RulerTriangle : public Polygon
{
public:
    RulerTriangle(QVector<QPointF> const & stuns);

    virtual void addPoint(QPointF const & pt);

    virtual void movePoint(QPointF const & pt);

    virtual bool commitPoint(QPointF const & pt);

    virtual bool canClose() override;

private:
    QPointF attachToLine(QPointF const & pt);

private:
    QVector<QPointF> stuns_;
    int curLine_ = 0; // update by attachToLine
    int lastLine_ = 0;
    int startLine_ = 0;
    int endLine_ = 0;
    QPointF start_;
    QPointF end_;
};

#endif // RULERTRIANGLE_H
