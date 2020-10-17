#ifndef LINE_H
#define LINE_H

#include "geometry2d.h"

class Line : public Geometry2D
{
    Q_OBJECT

    Q_PROPERTY(LineType lineType MEMBER lineType_)
    Q_PROPERTY(EndianType beginType MEMBER beginType_)
    Q_PROPERTY(EndianType endType MEMBER endType_)

public:
    enum LineType
    {
        Solid,
        Dash,
        Dot,
        DashDot,
        DashDotDot,
    };

    Q_ENUM(LineType)

    enum EndianType
    {
        None,
        Ball,
        SolidBall,
        Arrow,
        SolidArrow,
        HollowArrow,
        SharpArrow,
        Diamond,
        SolidDiamod,
        Box,
        SolidBox,
    };

    Q_ENUM(EndianType)

    class LineTypeToolButtons;
    class EndianTypeToolButtons;

public:
    Q_INVOKABLE Line(Resource * res);

    Q_INVOKABLE Line(Line const & o);

public:
    virtual bool move(int elem, QPointF const & pt) override;

    virtual QPainterPath visualPath() override;

    virtual QPainterPath contour() override;

private:
    static QPen linePen(LineType type, QColor color, qreal width);

    static void fillEndian(QPainterPath & ph, EndianType type, qreal width,
                           QPointF & pt, QPointF const & dir, bool & solid);

private:
    LineType lineType_;
    EndianType beginType_;
    EndianType endType_;
};

#endif // LINE_H
