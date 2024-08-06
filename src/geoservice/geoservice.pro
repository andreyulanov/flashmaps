TARGET = qtgeoservices_flashmaps
QT += location-private
CONFIG += c++2a

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryFlashMaps
load(qt_plugin)

QMAKE_CXXFLAGS += -O0
LIBS += -L$$PWD/../flashrender
INCLUDEPATH += $$PWD/../base
INCLUDEPATH += $$PWD/../flashrender

HEADERS += \
    ../base/flashbase.h \
    ../base/flashclass.h \
    ../base/flashclassmanager.h \
    ../base/flashdatetime.h \
    ../base/flashlocker.h \
    ../base/flashmap.h \
    ../base/flashobject.h \
    ../base/flashserialize.h \
    ../flashrender/flashrender.h \
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
    ../base/flashmap.cpp \
    ../base/flashobject.cpp \
    ../flashrender/flashrender.cpp \
    qgeomapreplyflashmaps.cpp \
    qgeoserviceproviderpluginflashmaps.cpp \
    qgeotiledmapflashmaps.cpp \
    qgeotiledmappingmanagerengineflashmaps.cpp \
    qgeotilefetcherflashmaps.cpp


OTHER_FILES += \
 flashmaps_plugin.json

