#ifndef RULERS_H
#define RULERS_H

#include "rulertool.h"
#include "linearruler.h"
#include "triangleruler.h"
#include "protractor.h"

#include <qexport.h>

REGISTER_CONTROL(RulerTool, "rulertool")
REGISTER_RESOURCE_VIEW_FACTORY(RulerFactory, Ruler, "rulertool")
REGISTER_RULER(LinearRuler, "linear")
REGISTER_RULER(TriangleRuler, "triangle,iso_triangle")
REGISTER_RULER(Protractor, "protractor")

#endif // RULERS_H
