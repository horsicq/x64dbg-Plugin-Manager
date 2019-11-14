QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = PluginManager
TEMPLATE = app

include(../build.pri)

SOURCES += \
    ../utils.cpp \
    dialogabout.cpp \
    dialogcreatemodule.cpp \
    dialoginstall.cpp \
    dialoginstallmodule.cpp \
    dialogoptions.cpp \
    dialogreload.cpp \
    dialogremove.cpp \
    guimainwindow.cpp \
    main_gui.cpp

HEADERS += \
    ../global.h \
    ../utils.h \
    dialogabout.h \
    dialogcreatemodule.h \
    dialoginstall.h \
    dialoginstallmodule.h \
    dialogoptions.h \
    dialogreload.h \
    dialogremove.h \
    guimainwindow.h

FORMS += \
    dialogabout.ui \
    dialogcreatemodule.ui \
    dialoginstall.ui \
    dialoginstallmodule.ui \
    dialogoptions.ui \
    dialogreload.ui \
    dialogremove.ui \
    guimainwindow.ui

win32 {
    RC_ICONS = ../icons/main.ico
}
