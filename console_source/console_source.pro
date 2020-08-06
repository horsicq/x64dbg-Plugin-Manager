QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = x64plgmnrc

include(../build.pri)

SOURCES += \
    ../createmoduleprocess.cpp \
    ../getfilefromserverprocess.cpp \
    ../installmoduleprocess.cpp \
    ../removemoduleprocess.cpp \
    ../utils.cpp \
    main_console.cpp \
    consoleoutput.cpp

HEADERS += \
    ../createmoduleprocess.h \
    ../getfilefromserverprocess.h \
    ../global.h \
    ../installmoduleprocess.h \
    ../removemoduleprocess.h \
    ../utils.h \
    consoleoutput.h

!contains(XCONFIG, xarchive) {
    XCONFIG += xarchive
    include(../XArchive/xarchive.pri)
}

!contains(XCONFIG, xgithub) {
    XCONFIG += xgithub
    include(../XGithub/xgithub.pri)
}

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include(../XOptions/xoptions.pri)
}
