include("../config.pri")

TEMPLATE = lib
TARGET = db
INCLUDEPATH += .

CONFIG += console
CONFIG += staticlib

QT -= gui core
CONFIG += c++14

HEADERS += \
    db.h \
    dbschema.h \


SOURCES += \
    db.cpp \
    dbschema.cpp \


OTHER_FILES += \
    ../db/schema.sql
