#ifndef REGULARPRISMOID_H
#define REGULARPRISMOID_H

#include "polyhedron.h"

#include <core/optiontoolbuttons.h>

class RegularPrismoid : public Polyhedron
{
    Q_OBJECT
    Q_PROPERTY(int edges READ edges WRITE setEdges)

public:
    Q_INVOKABLE RegularPrismoid(Resource * res);

    Q_INVOKABLE RegularPrismoid(RegularPrismoid const & o);

public slots:
    void setEdges(int n);

    int edges() { return nEdges_; }

public:
    virtual int pointCount() override;

    virtual QVector3D point(int index) override;

    virtual bool moveElememt(int elem, const QPointF &pt) override;

    virtual void sync() override;

    virtual QVector<QPointF> movePoints() override;

protected:
    RegularPrismoid(Resource * res, qreal ratio);

    virtual void makeLines();

protected:
    int nEdges_ = 0;
    qreal ratio_ = 0.5;
    QPointF vAngleStep_;
    QPointF vAngleInit_;
};

class EdgesToolButtons : public OptionToolButtons
{
public:
    EdgesToolButtons(QString const & title);
protected:
    virtual QString buttonTitle(const QVariant &value) override;
private:
    static QString buttonTitle(QString const & title, int n);
private:
    QString title_;
};

#endif // REGULARPRISMOID_H
