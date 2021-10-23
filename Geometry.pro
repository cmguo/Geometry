QT += gui widgets quick qml quickwidgets

TEMPLATE = lib
DEFINES += GEOMETRY_LIBRARY
CONFIG += plugin

CONFIG += c14

include($$(applyCommonConfig))
include($$(applyConanPlugin))

include(../config.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    geometryplugin.cpp \
    geometrytool.cpp

HEADERS += \
    Geometry_global.h \
    exports.h \
    geometryplugin.h \
    geometrytool.h

RESOURCES += \
    res/Geometry.qrc

DISTFILES += \
    Geometry.json

include(base/base.pri)
include(geometry2ds/geometry2ds.pri)
include(geometry3ds/geometry3ds.pri)
include(rulers/rulers.pri)
include(algorithm/algorithm.pri)

includes.files = $$PWD/*.h
win32 {
    includes.path = $$[QT_INSTALL_HEADERS]/Geometry
    target.path = $$[QT_INSTALL_LIBS]
}
INSTALLS += includes

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

exists($$PWD/../OpenCV/opencv/build) {
    DEFINES += HAS_OPENCV
    win32: {
        LIBS += -L$$PWD/../OpenCV/opencv/build/lib
        CONFIG(debug, debug|release): -lopencv_world454d
        CONFIG(release, debug|release): -lopencv_world454
    }
    INCLUDEPATH += $$PWD/../OpenCV/opencv/build/include
}
