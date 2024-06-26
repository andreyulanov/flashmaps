QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion

INCLUDEPATH += $$PWD/../base

SOURCES += \
    ../base/kbase.cpp \
    ../base/klocker.cpp \
    ../base/kpack.cpp \
    ../base/kclass.cpp \
    ../base/kobject.cpp \
    ../base/kclassmanager.cpp \
    main.cpp

HEADERS += \
    ../lib/kbase.h \
    ../base/klocker.h \
    ../base/kpack.h \
    ../base/kserialize.h \
    ../base/kclass.h \
    ../base/kclassmanager.cpp



