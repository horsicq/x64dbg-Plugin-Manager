QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = PluginManager
TEMPLATE = app

include(../build.pri)

SOURCES += \
    ../getfilefromserverprocess.cpp \
    ../installmoduleprocess.cpp \
    ../removemoduleprocess.cpp \
    ../utils.cpp \
    ../createmoduleprocess.cpp \
    dialogabout.cpp \
    dialogcreatemodule.cpp \
    dialogcreatemoduleprocess.cpp \
    dialoggetfilefromserver.cpp \
    dialoginstallmodule.cpp \
    dialoginstallmoduleprocess.cpp \
    dialogoptions.cpp \
    dialogremovemodule.cpp \
    dialogremovemoduleprocess.cpp \
    guimainwindow.cpp \
    main_gui.cpp

HEADERS += \
    ../getfilefromserverprocess.h \
    ../global.h \
    ../installmoduleprocess.h \
    ../removemoduleprocess.h \
    ../utils.h \
    ../createmoduleprocess.h \
    dialogabout.h \
    dialogcreatemodule.h \
    dialogcreatemoduleprocess.h \
    dialoggetfilefromserver.h \
    dialoginstallmodule.h \
    dialoginstallmoduleprocess.h \
    dialogoptions.h \
    dialogremovemodule.h \
    dialogremovemoduleprocess.h \
    guimainwindow.h

FORMS += \
    dialogabout.ui \
    dialogcreatemodule.ui \
    dialogcreatemoduleprocess.ui \
    dialoggetfilefromserver.ui \
    dialoginstallmodule.ui \
    dialoginstallmoduleprocess.ui \
    dialogoptions.ui \
    dialogreload.ui \
    dialogremovemodule.ui \
    dialogremovemoduleprocess.ui \
    guimainwindow.ui

!contains(XCONFIG, xarchive) {
    XCONFIG += xarchive
    include(../XArchive/xarchive.pri)
}

win32 {
    RC_ICONS = ../icons/main.ico
}
