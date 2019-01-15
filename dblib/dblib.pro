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
    mysql_backend.h


SOURCES += \
    db.cpp \
    dbschema.cpp \
    mysql_backend.cpp


OTHER_FILES += \
    ../db/schema.sql
