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
    ../base/flashbase.h \
    ../base/flashclass.h \
    ../base/flashclassmanager.h \
    ../base/flashdatetime.h \
    ../base/flashlocker.h \
    ../base/flashobject.h \
    ../base/flashpack.h \
    ../base/flashserialize.h \
    krender/flashrender.h \
    krender/flashrenderpack.h \
    qgeomapreplyflashmaps.h \
    qgeoserviceproviderpluginflashmaps.h \
    qgeotiledmapflashmaps.h \
    qgeotiledmappingmanagerengineflashmaps.h \
    qgeotilefetcherflashmaps.h

SOURCES += \
    ../base/flashbase.cpp \
    ../base/flashclass.cpp \
    ../base/flashclassmanager.cpp \
    ../base/flashdatetime.cpp \
    ../base/flashlocker.cpp \
    ../base/flashobject.cpp \
    ../base/flashpack.cpp \
    krender/flashrender.cpp \
    krender/flashrenderpack.cpp \
    qgeomapreplyflashmaps.cpp \
    qgeoserviceproviderpluginflashmaps.cpp \
    qgeotiledmapflashmaps.cpp \
    qgeotiledmappingmanagerengineflashmaps.cpp \
    qgeotilefetcherflashmaps.cpp


OTHER_FILES += \
 flashmaps_plugin.json

