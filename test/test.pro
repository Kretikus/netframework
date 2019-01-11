include("../config.pri")

TEMPLATE = app

CONFIG += c++14
CONFIG += console
CONFIG -= app_bundle

QT -= gui
QT += testlib

INCLUDEPATH += ..
INCLUDEPATH += ../lib
win32 {
  LIBS += ../lib/debug/netf.lib
  LIBS += ../dblib/debug/db.lib
} else {
  LIBS += ../lib/libnetf.a
  LIBS += ../dblib/libdb.a
}
# Specify that we depend on the library (which, logically would be implicit from
# the fact that we are linking to it)
PRE_TARGETDEPS += ../lib/libnetf.a

LIBS += $${EXT_LIBS}


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
    handlertest.cpp \
    logtest.cpp \
    dbtest.cpp \

TARGET = test

