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

    static constexpr int SolidShape = 1 << 8;
    static constexpr int AlignLeft = 1 << 9;
    static constexpr int AlignRight = 1 << 10;
    static constexpr int AlignBoth = AlignLeft | AlignRight;

    enum EndianType
    {
        None = 0,
        Ball = 1,
        SolidBall = 2 | SolidShape,
        Arrow = 3 | AlignRight,
        SolidArrow = 4 | SolidShape | AlignRight,
        HollowArrow = 5 | AlignRight,
        SharpArrow = 6 | SolidShape | AlignRight,
        Diamond = 7 | AlignBoth,
        SolidDiamod = 8 | SolidShape | AlignBoth,
        Box = 9,
        SolidBox = 10 | SolidShape,
    };

    Q_ENUM(EndianType)

    class LineTypeToolButtons;
    class EndianTypeToolButtons;

public:
    Q_INVOKABLE Line(Resource * res);

    Q_INVOKABLE Line(Line const & o);

public:
    virtual bool moveElememt(int elem, QPointF const & pt) override;

    virtual QPainterPath visualPath() override;

    virtual QPainterPath contour() override;

private:
    static void fillEndian(QPainterPath & ph, EndianType type, qreal width,
                           QPointF & pt, QPointF const & dir, bool & solid);

private:
    LineType lineType_;
    EndianType beginType_;
    EndianType endType_;
};

#endif // LINE_H
