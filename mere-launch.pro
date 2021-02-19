QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += \
        src/desktoplauncher.cpp \
        src/launchapp.cpp \
        src/main.cpp

HEADERS += \
        src/desktoplauncher.h \
        src/launchapp.h

INCLUDEPATH += /usr/local/include

LIBS += -lmere-xdg -lmere-utils


#
# Install
#
unix
{
    target.path = /usr/local/bin
    INSTALLS += target
}
