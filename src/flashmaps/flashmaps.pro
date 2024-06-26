TEMPLATE = app

QT += location

SOURCES += main.cpp

RESOURCES += qml.qrc

ANDROID_EXTRA_LIBS = $$PWD/../../build-main-Android_Qt_5_15_9_qt_5_15_9_android_dbus_Clang_Multi_Abi-Release/geoservice/plugins/geoservices/libplugins_geoservices_qtgeoservices_flashmaps_armeabi-v7a.so

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
