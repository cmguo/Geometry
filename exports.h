#ifndef EXPORTS_H
#define EXPORTS_H

#include "geometry2ds/geometry2ds.h"
#include "geometry3ds/geometry3ds.h"
#include "geometrycontrol.h"
#include "geometrytool.h"

REGISTER_CONTROL(GeometryControl, "geometry2d,geometry3d")
REGISTER_CONTROL(GeometryTool, "geometry2dtool,geometry3dtool");

#endif // EXPORTS_H
