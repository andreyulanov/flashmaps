QT       += widgets
QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion
QMAKE_CXXFLAGS += -O0

INCLUDEPATH += /usr/include/gisdesigner
INCLUDEPATH += $$PWD/../base

LIBS += -lmapcomponents -lqdmapacces
SOURCES += \
    ../base/flashbase.cpp \
    ../base/flashlocker.cpp \
    ../base/flashpack.cpp \
    ../base/flashclass.cpp \
    ../base/flashobject.cpp \
    ../base/flashclassmanager.cpp \
    flashpanclassmanager.cpp \
    main.cpp

HEADERS += \
    ../base/flashbase.h \
    ../base/flashlocker.h \
    ../base/flashpack.h \
    ../base/flashserialize.h \
    ../base/flashclass.h \
    ../base/flashclassmanager.h \
 flashpanclassmanager.h \
 flashpanclass.h



