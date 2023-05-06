QT += core network gui widgets

TARGET = JEducationServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += debug_and_release

HEADERS += \
    dataTypes.h \
    meeting.h \
    server.h \
    serverworker.h \
    serverWindow.h

SOURCES += \
    dataTypes.cpp \
    meeting.cpp \
    server.cpp \
    servermain.cpp \
    serverworker.cpp \
    serverWindow.cpp

FORMS += \
    window.ui
