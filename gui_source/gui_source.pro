QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = PluginManager
TEMPLATE = app

include(../build.pri)

SOURCES += \
    ../installmoduleprocess.cpp \
    ../removemoduleprocess.cpp \
    ../utils.cpp \
    ../createmoduleprocess.cpp \
    dialogabout.cpp \
    dialogcreatemodule.cpp \
    dialogcreatemoduleprocess.cpp \
    dialoginstallmodule.cpp \
    dialoginstallmoduleprocess.cpp \
    dialogoptions.cpp \
    dialogreload.cpp \
    dialogremovemodule.cpp \
    dialogremovemoduleprocess.cpp \
    guimainwindow.cpp \
    main_gui.cpp

HEADERS += \
    ../global.h \
    ../installmoduleprocess.h \
    ../removemoduleprocess.h \
    ../utils.h \
    ../createmoduleprocess.h \
    dialogabout.h \
    dialogcreatemodule.h \
    dialogcreatemoduleprocess.h \
    dialoginstallmodule.h \
    dialoginstallmoduleprocess.h \
    dialogoptions.h \
    dialogreload.h \
    dialogremovemodule.h \
    dialogremovemoduleprocess.h \
    guimainwindow.h

FORMS += \
    dialogabout.ui \
    dialogcreatemodule.ui \
    dialogcreatemoduleprocess.ui \
    dialoginstallmodule.ui \
    dialoginstallmoduleprocess.ui \
    dialogoptions.ui \
    dialogreload.ui \
    dialogremovemodule.ui \
    dialogremovemoduleprocess.ui \
    guimainwindow.ui

win32 {
    RC_ICONS = ../icons/main.ico
}
