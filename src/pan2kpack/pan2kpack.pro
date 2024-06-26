QT       += widgets
QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion
QMAKE_CXXFLAGS += -O0

INCLUDEPATH += /usr/include/gisdesigner
INCLUDEPATH += $$PWD/../base

LIBS += -lmapcomponents -lqdmapacces
SOURCES += \
    ../base/kbase.cpp \
    ../base/klocker.cpp \
    ../base/kpack.cpp \
    ../base/kclass.cpp \
    ../base/kobject.cpp \
    ../base/kclassmanager.cpp \
    kpanclassmanager.cpp \
    main.cpp

HEADERS += \
    ../base/kbase.h \
    ../base/klocker.h \
    ../base/kpack.h \
    ../base/kserialize.h \
    ../base/kclass.h \
    ../base/kclassmanager.h \
 kpanclass.h \
 kpanclassmanager.h



