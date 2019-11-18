#ifndef GEOMETRY3DS_H
#define GEOMETRY3DS_H

#include "geometry3d.h"
#include "cuboid.h"
#include "cube.h"
#include "frustumcone.h"
#include "cone.h"
#include "cylinder.h"

REGISTER_RESOURCE_VIEW_FACTORY(Geometry3DFactory, Geometry3D, "geometry3d")
REGISTER_GEOMETRY_3D(Cuboid, "cuboid")
REGISTER_GEOMETRY_3D(Cube, "cube")
REGISTER_GEOMETRY_3D(FrustumCone, "frustumcone")
REGISTER_GEOMETRY_3D(Cone, "cone")
REGISTER_GEOMETRY_3D(Cylinder, "cylinder")

#endif // GEOMETRY3DS_H
