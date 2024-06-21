TARGET = qtgeoservices_flashmaps
QT += location-private
CONFIG += c++2a

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryFlashMaps
load(qt_plugin)

QMAKE_CXXFLAGS += -O0

HEADERS += \
    krender/kbase.h \
    krender/kclass.h \
    krender/kclassmanager.h \
    krender/kdatetime.h \
    krender/klocker.h \
    krender/kobject.h \
    krender/kpack.h \
    krender/krender.h \
    krender/krenderpack.h \
    krender/kserialize.h \
    qgeoserviceproviderpluginbingmaps.h \
    qgeomapreplybingmaps.h \
    qgeotiledmapbingmaps.h \
    qgeotiledmappingmanagerenginebingmaps.h \
    qgeotilefetcherbingmaps.h

SOURCES += \
    krender/kbase.cpp \
    krender/kclass.cpp \
    krender/kclassmanager.cpp \
    krender/kdatetime.cpp \
    krender/klocker.cpp \
    krender/kobject.cpp \
    krender/kpack.cpp \
    krender/krender.cpp \
    krender/krenderpack.cpp \
    qgeoserviceproviderpluginbingmaps.cpp \
    qgeomapreplybingmaps.cpp \
    qgeotiledmapbingmaps.cpp \
    qgeotiledmappingmanagerenginebingmaps.cpp \
    qgeotilefetcherbingmaps.cpp


OTHER_FILES += \
    bingmaps_plugin.json

