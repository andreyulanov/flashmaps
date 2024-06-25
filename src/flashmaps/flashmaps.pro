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
    qgeomapreplyflashmaps.h \
    qgeoserviceproviderpluginflashmaps.h \
    qgeotiledmapflashmaps.h \
    qgeotiledmappingmanagerengineflashmaps.h \
    qgeotilefetcherflashmaps.h

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
    qgeomapreplyflashmaps.cpp \
    qgeoserviceproviderpluginflashmaps.cpp \
    qgeotiledmapflashmaps.cpp \
    qgeotiledmappingmanagerengineflashmaps.cpp \
    qgeotilefetcherflashmaps.cpp


OTHER_FILES += \
 flashmaps_plugin.json

