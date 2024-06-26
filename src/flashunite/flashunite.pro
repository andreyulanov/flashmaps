QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion
QMAKE_CXXFLAGS += -O0

INCLUDEPATH += $$PWD/../base

SOURCES += \
    ../base/flashbase.cpp \
    ../base/flashlocker.cpp \
    ../base/flashmap.cpp \
    ../base/flashclass.cpp \
    ../base/flashobject.cpp \
    ../base/flashclassmanager.cpp \
    main.cpp

HEADERS += \
    ../base/flashbase.h \
    ../base/flashlocker.h \
    ../base/flashmap.h \
    ../base/flashserialize.h \
    ../base/flashclass.h \
    ../base/flashclassmanager.cpp



