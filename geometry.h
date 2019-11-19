#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "core/resourceview.h"

#include <QtPromise>

#include <QVector>
#include <QPointF>
#include <QColor>

class Geometry : public ResourceView
{

    Q_OBJECT

    Q_PROPERTY(QColor color MEMBER color_)
    Q_PROPERTY(qreal width MEMBER width_)

public:
    virtual bool empty() const;

    virtual void clear();

    virtual void addPoint(QPointF const & pt);

    virtual void movePoint(QPointF const & pt);

    virtual bool commit(QPointF const & pt);

    virtual bool canFinish();

    virtual void finish(const QPointF &c);

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
    virtual QPainterPath path() = 0;

    virtual QVector<QPointF> movePoints();

public:
    virtual QtPromise::QPromise<void> load();

protected:
    Geometry(Resource * res);

    Geometry(QString const & type);

    Geometry(Geometry const & o);

protected:
    static constexpr qreal HIT_DIFF = 10.0;

    static constexpr qreal HIT_DIFF_DIFF = 100.0;

protected:
    QVector<QPointF> points_;
    bool dirty_;

protected:
    QColor color_;
    qreal width_;
};

#endif // GEOMETRY_H
