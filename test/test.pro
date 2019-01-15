include("../config.pri")

TEMPLATE = app

CONFIG += c++14
CONFIG += console
CONFIG -= app_bundle

QT -= gui
QT += core
QT += testlib

INCLUDEPATH += ..
INCLUDEPATH += ../lib
win32 {
  # Specify that we depend on the library (which, logically would be implicit from
  # the fact that we are linking to it)
  PRE_TARGETDEPS += ../lib/debug/netf.lib ../dblib/debug/db.lib

  LIBS += ../lib/debug/netf.lib
  LIBS += ../dblib/debug/db.lib

  INCLUDEPATH += debug/
} else {
  # Specify that we depend on the library (which, logically would be implicit from
  # the fact that we are linking to it)
  PRE_TARGETDEPS += ../lib/libnetf.a


  LIBS += ../lib/libnetf.a
  LIBS += ../dblib/libdb.a
}

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

