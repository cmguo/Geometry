#ifndef QRTHOPRISM_H
#define QRTHOPRISM_H

#include "regularprismoid.h"

class Qrthoprism : public RegularPrismoid
{
    Q_OBJECT
public:
    Q_INVOKABLE Qrthoprism(Resource * res);

    Q_INVOKABLE Qrthoprism(Qrthoprism const & o);

public:
    virtual int pointCount() override;

    virtual QVector3D point(int index) override;

    virtual bool moveElememt(int elem, const QPointF &pt) override;

protected:
    virtual void makeLines() override;
};

#endif // QRTHOPRISM_H
