#ifndef RULER_H
#define RULER_H

#include <core/resourceview.h>
#include <core/resourcefactory.h>

class Ruler : public ResourceView
{
    Q_OBJECT
    Q_CLASSINFO("version", "1.0")

public:
    Ruler(Resource * res, Flags flags = {}, Flags clearFlags = {});

public:
    // Unit length vector
    virtual QPointF adjustDirection(QRectF & adjust);

    virtual void updateShape();

    virtual QVector<QPointF> getControlPositions() = 0;

    virtual void onDraw(QPainter * painter);

protected:
    enum TickFlags {
        Anticlockwise = 1,
        CrossLittenEndian = 2,
        NeedRotate = 4,
        ClipByShape = 8
    };

    QPointF adjust(QPointF const & offset);

    // 刻度线，
    QVector<QPointF> tickMarkPoints(QPointF const & from, QPointF const & to, int flags);

    void drawTickMarks(QPainter * painter, QPointF const & from, QPointF const & to, int flags);

protected:
    static constexpr qreal Unit = 5; // 每毫米px

protected:
    friend class RulerItem;

    qreal width_;
    qreal height_;
    qreal minWidth_ = 300;
    QPointF rotateCenter_;
    QPainterPath shape_;
    QPainterPath shape1_;
    QPainterPath shape2_;
};

class RulerFactory : ResourceFactory
{
    Q_OBJECT
public:
    Q_INVOKABLE RulerFactory();

public:
    virtual ResourceView * create(Resource * res) override;

    virtual QUrl newUrl(const QByteArray &type) const override;
};

#define REGISTER_RULER(ctype, type) \
    REGISTER_RESOURCE_VIEW_WITH_FACTORY(Ruler, ctype, type)

#endif // RULER_H
