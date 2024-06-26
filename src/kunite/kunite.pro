QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion

INCLUDEPATH += $$PWD/../base

SOURCES += \
    ../base/flashbase.cpp \
    ../base/flashlocker.cpp \
    ../base/flashpack.cpp \
    ../base/flashclass.cpp \
    ../base/flashobject.cpp \
    ../base/flashclassmanager.cpp \
    main.cpp

HEADERS += \
    ../base/flashbase.h \
    ../base/flashlocker.h \
    ../base/flashpack.h \
    ../base/flashserialize.h \
    ../base/flashclass.h \
    ../base/flashclassmanager.cpp



