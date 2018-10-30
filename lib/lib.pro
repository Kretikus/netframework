include("../config.pri")

TEMPLATE = lib
TARGET = netf
INCLUDEPATH += .

CONFIG += console
CONFIG += staticlib

QT -= gui core

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += \
    client.h \
    clientnet.h \
    consthelpers.hpp \
    coreproto.h \
    handlerbase.h \
    log.h \
    netbase.h \
    netutil.h \
    protocolbase.h \
    remotecall.h \
    retvalserialisation.h \
    serialisation.h \
    server.h \
    workerthread.h \

SOURCES += \
    client.cpp \
    clientnet.cpp \
    handlerbase.cpp \
    log.cpp \
    main.cpp \
    netbase.cpp \
    netutil.cpp \
    remotecall.cpp \
    serialisation.cpp \
    server.cpp \
    workerthread.cpp \

CONFIG += c++14
