TEMPLATE = app

QT += location

SOURCES += main.cpp \
 ../base/flashbase.cpp \
 ../base/flashclass.cpp \
 ../base/flashclassmanager.cpp \
 ../base/flashdatetime.cpp \
 ../base/flashlocker.cpp \
 ../base/flashmap.cpp \
 ../base/flashobject.cpp \
 ../flashrender/flashrender.cpp \
 chatmap.cpp \
 downloadmanager.cpp

RESOURCES += qml.qrc

ANDROID_EXTRA_LIBS = $$PWD/../../build-main-Android_Qt_5_15_9_qt_5_15_9_android_dbus_Clang_Multi_Abi-Release/geoservice/plugins/geoservices/libplugins_geoservices_qtgeoservices_flashmaps_armeabi-v7a.so

CONFIG += c++2a
QMAKE_CXXFLAGS += -O0
LIBS += -L$$PWD/../flashrender
INCLUDEPATH += $$PWD/../base
INCLUDEPATH += $$PWD/../flashrender

DISTFILES += \
 android/AndroidManifest.xml \
 android/build.gradle \
 android/gradle.properties \
 android/gradle/wrapper/gradle-wrapper.jar \
 android/gradle/wrapper/gradle-wrapper.properties \
 android/gradlew \
 android/gradlew.bat \
 android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

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
 chatmap.h \
 downloadmanager.h
