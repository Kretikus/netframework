TEMPLATE = app

CONFIG += c++14
CONFIG += console
CONFIG -= appbundle

QT -= gui
QT += testlib

INCLUDEPATH += ../lib
win32 {
  LIBS += ../lib/debug/netf.lib
} else {
  LIBS += ../lib/libnetf.a
}
# Specify that we depend on the library (which, logically would be implicit from
# the fact that we are linking to it)
PRE_TARGETDEPS += ../lib/libnetf.a


HEADERS += \
    testutil.h \
    serverclienttest.h \
    helpertest.h

SOURCES += \
    maintest.cpp \
    testutil.cpp \
    threadtest.cpp \
    serverclienttest.cpp \
    helpertest.cpp \
    handlertest.cpp

TARGET = test

