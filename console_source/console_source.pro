QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = PluginManagerConsole

include(../build.pri)

SOURCES += \
    main_console.cpp

!contains(XCONFIG, xarchive) {
    XCONFIG += xarchive
    include(../XArchive/xarchive.pri)
}
