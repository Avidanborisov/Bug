QT       -= core gui

TARGET = stdlib
TEMPLATE = lib
CONFIG += staticlib

include(../build.pri)

SOURCES += \
    string.cpp \
    memops.cpp \
    cxxabi.cpp \
    heap.cpp \
    console.cpp

HEADERS += \
    vector.hpp \
    bitset.hpp \
    string.hpp \
    uniquepointer.hpp \
    optional.hpp \
    array.hpp \
    utility.hpp \
    new.hpp \
    assert.hpp \
    memops.hpp \
    cxxabi.hpp \
    math.hpp \
    heap.hpp \
    console.hpp
