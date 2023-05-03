QT       += core gui network widgets qml quick pdf svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Controller/controller.cpp \
    ask.cpp \
    brainstorm.cpp \
    choosemode.cpp \
    dialog.cpp \
    info.cpp \
    learning.cpp \
    main.cpp \
    main.cpp \
    mainwindow.cpp \
    other_questions.cpp \
    rating.cpp

HEADERS += \
    Controller/controller.h \
    ask.h \
    brainstorm.h \
    choosemode.h \
    dataTypes.h \
    dialog.h \
    info.h \
    learning.h \
    mainwindow.h \
    other_questions.h \
    rating.h

FORMS += \
    ask.ui \
    brainstorm.ui \
    choosemode.ui \
    dialog.ui \
    info.ui \
    learning.ui \
    mainwindow.ui \
    other_questions.ui \
    rating.ui

win32:RC_FILE = appLogo.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    pdfViewer/resources/zoom-in.svg \
    pdfViewer/resources/zoom-out.svg \
    pdfViewer/viewer.qml

RESOURCES += \
    pdfViewer/viewer.qrc

macos:QMAKE_INFO_PLIST = pdfViewer/resources/macos/Info.plist
macos:ICON = pdfViewer/resources/multipage.icns
