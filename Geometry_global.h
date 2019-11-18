#ifndef GEOMETRY_GLOBAL_H
#define GEOMETRY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GEOMETRY_LIBRARY)
#  define GEOMETRY_EXPORT Q_DECL_EXPORT
#else
#  define GEOMETRY_EXPORT Q_DECL_IMPORT
#endif

#endif // GEOMETRY_GLOBAL_H
