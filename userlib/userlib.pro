QT       -= core gui

TARGET = userlib
TEMPLATE = lib
CONFIG += staticlib

include(../build.pri)

INCLUDEPATH += $$PWD/../stdlib
DEPENDPATH += $$PWD/../stdlib

HEADERS += \
    system.hpp

SOURCES += \
    system.cpp \
    syscall.S \
    runtime.cpp
