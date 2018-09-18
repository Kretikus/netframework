TEMPLATE = app

CONFIG += c++14
CONFIG += console
CONFIG -= appbundle

QT -= gui
QT += testlib

INCLUDEPATH += ../lib

LIBS += ../lib/libnetf.a
# Specify that we depend on the library (which, logically would be implicit from
# the fact that we are linking to it)
PRE_TARGETDEPS += ../lib/libnetf.a


HEADERS += \
    testutil.h

SOURCES += \
    maintest.cpp \
    testutil.cpp \
    threadtest.cpp

TARGET = test

