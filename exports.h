#ifndef EXPORTS_H
#define EXPORTS_H

#include "geometry2ds/geometry2ds.h"
#include "geometry3ds/geometry3ds.h"
#include "rulers/rulers.h"
#include "geometrycontrol.h"
#include "geometrytool.h"
#include "geometryhelper.h"

REGISTER_CONTROL(GeometryControl, "geometry2d,geometry3d")
REGISTER_CONTROL(GeometryTool, "geometry2dtool,geometry3dtool");

static QExport<GeometryTools> export_tools(QPart::shared);
static QExport<GeometryHelper> export_helper(QPart::shared);

#endif // EXPORTS_H
