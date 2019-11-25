#ifndef GEOMETRY2DS_H
#define GEOMETRY2DS_H

#include "geometry2d.h"
#include "circle.h"
#include "ellipse.h"
#include "sector.h"
#include "isoscelestriangle.h"
#include "righttriangle.h"
#include "rectangle.h"
#include "square.h"
#include "polygon.h"
#include "arbitrarypolygon.h"
#include "regularpolygon.h"
#include "righttrapezoid.h"
#include "isopopetrapezoid.h"
#include "parallelogram.h"
#include "diamond.h"
#include "line.h"

REGISTER_RESOURCE_VIEW_FACTORY(Geometry2DFactory, Geometry2D, "geometry2d")
REGISTER_GEOMETRY_2D(Ellipse, "ellipse")
REGISTER_GEOMETRY_2D(Circle, "circle")
REGISTER_GEOMETRY_2D(Sector, "sector")
REGISTER_GEOMETRY_2D(IsoscelesTriangle, "isoscelestriangle")
REGISTER_GEOMETRY_2D(RightTriangle, "righttriangle")
REGISTER_GEOMETRY_2D(IsopopeTrapezoid, "isopopetrapezoid")
REGISTER_GEOMETRY_2D(RightTrapezoid, "righttrapezoid")
REGISTER_GEOMETRY_2D(Parallelogram, "parallelogram")
REGISTER_GEOMETRY_2D(Diamond, "diamond")
REGISTER_GEOMETRY_2D(Rectangle, "rectangle")
REGISTER_GEOMETRY_2D(Square, "square")
REGISTER_GEOMETRY_2D(ArbitraryPolygon, "arbitrarypolygon")
REGISTER_GEOMETRY_2D(Line,
                  "line.o-,line.o-b,dashline.b-ha,dotline.so-ra,dashdotline.so-a");
REGISTER_GEOMETRY_2D(RegularPolygon,
                  "regularpolygon3,"
                  "regularpolygon4,"
                  "regularpolygon5,regularpolygon5.2,"
                  /*"regularpolygon6,"
                  "regularpolygon7,regularpolygon7.2,regularpolygon7.3,"
                  "regularpolygon8,regularpolygon8.3,"
                  "regularpolygon9,regularpolygon9.2,regularpolygon9.4,"*/)

#endif // GEOMETRY2DS_H
