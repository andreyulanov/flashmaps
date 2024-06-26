TARGET = qtgeoservices_flashmaps
QT += location-private
CONFIG += c++2a

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryFlashMaps
load(qt_plugin)

QMAKE_CXXFLAGS += -O0
LIBS += -L$$PWD/krender
INCLUDEPATH += $$PWD/../base

HEADERS += \
    ../base/kbase.h \
    ../base/kclass.h \
    ../base/kclassmanager.h \
    ../base/kdatetime.h \
    ../base/klocker.h \
    ../base/kobject.h \
    ../base/kpack.h \
    ../base/kserialize.h \
    krender/krender.h \
    krender/krenderpack.h \
    qgeomapreplyflashmaps.h \
    qgeoserviceproviderpluginflashmaps.h \
    qgeotiledmapflashmaps.h \
    qgeotiledmappingmanagerengineflashmaps.h \
    qgeotilefetcherflashmaps.h

SOURCES += \
    ../base/kbase.cpp \
    ../base/kclass.cpp \
    ../base/kclassmanager.cpp \
    ../base/kdatetime.cpp \
    ../base/klocker.cpp \
    ../base/kobject.cpp \
    ../base/kpack.cpp \
    krender/krender.cpp \
    krender/krenderpack.cpp \
    qgeomapreplyflashmaps.cpp \
    qgeoserviceproviderpluginflashmaps.cpp \
    qgeotiledmapflashmaps.cpp \
    qgeotiledmappingmanagerengineflashmaps.cpp \
    qgeotilefetcherflashmaps.cpp


OTHER_FILES += \
 flashmaps_plugin.json

