QT += core network gui widgets

QT += sql

TARGET = JEducationServer
TEMPLATE = app


DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += debug_and_release

HEADERS += \
    dataTypes.h \
    database.h \
    meeting.h \
    server.h \
    serverworker.h \
    serverWindow.h

SOURCES += \
    dataTypes.cpp \
    database.cpp \
    meeting.cpp \
    server.cpp \
    servermain.cpp \
    serverworker.cpp \
    serverWindow.cpp

FORMS += \
    window.ui
