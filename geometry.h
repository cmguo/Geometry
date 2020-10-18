#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Geometry_global.h"
#include <core/resourceview.h>

#include <QtPromise>

#include <QVector>
#include <QPointF>
#include <QColor>

class ToolButton;

class GEOMETRY_EXPORT Geometry : public ResourceView
{

    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(qreal width READ width WRITE setWidth)

public:
    static constexpr Flag DrawAttach = CustomFlag;

public:
    virtual bool empty() const;

    virtual bool finished() const;

    virtual void clear();

    virtual void addPoint(QPointF const & pt);

    virtual void movePoint(QPointF const & pt);

    virtual bool commitPoint(QPointF const & pt);

    virtual bool moveTempPoint(QPointF const & pt);

    virtual void sync();

    virtual bool canFinish();

    virtual void finish(const QPointF &c);

    void scale(qreal scale);

public:
    QColor color() const
    {
        return color_;
    }

    void setColor(QColor color);

    qreal width() const
    {
        return width_;
    }

    void setWidth(qreal width);

    void init();

signals:
    void changed();

public:
    /*
     * return element (>=0) under point pt
     *  adjust pt to real element position
     */
    virtual int hit(QPointF & pt);

    /*
     * move element elem to point pt
     *  pt is adjusted with some diff as hit()
     */
    virtual bool move(int elem, QPointF const & pt);

public:
    virtual QPainterPath graphPath();

    virtual QPainterPath textPath();

    virtual QPainterPath contour();

    // include graphPath & textPath
    virtual QPainterPath visualPath();

    virtual QVector<QPointF> movePoints();

    // return QQuickShapePath (Qml ShapePath)
    virtual QObject * toQuickPath(QObject * context);

public:
    virtual QtPromise::QPromise<void> load();

    virtual bool setOption(QByteArray const & key, QVariant value) override;

protected:
    Geometry(Resource * res, Flags flags = {}, Flags clearFlags = {});

    Geometry(Geometry const & o);

    static void fillQuickPath(QObject * path, QPainterPath const & ph);

protected:
    QVector<QPointF> points_;
    bool dirty_;

protected:
    QColor color_;
    qreal width_;
};

#endif // GEOMETRY_H
