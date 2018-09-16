TEMPLATE = app

CONFIG += console
CONFIG -= bundle
QT -= gui
QT += testlib

INCLUDEPATH += ../src

HEADERS += \
    testutil.h

SOURCES += \
    maintest.cpp \
    testutil.cpp
