#ifndef RULERLINE_H
#define RULERLINE_H

#include "geometry2ds/line.h"

class RulerLine : public Line
{
public:
    RulerLine(QLineF const & line);

    virtual void addPoint(QPointF const & pt);

    virtual void movePoint(QPointF const & pt);

    virtual bool commitPoint(QPointF const & pt);

private:
    QPointF attachToLine(QPointF const & pt);

private:
    QLineF line_;
};


#endif // RULERLINE_H
