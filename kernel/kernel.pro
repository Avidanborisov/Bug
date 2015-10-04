TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LINK = i686-elf-g++
QMAKE_CC   = i686-elf-gcc
QMAKE_CXX  = i686-elf-g++
OBJCOPY    = i686-elf-objcopy

QMAKE_CFLAGS    = -ffreestanding -Wall -Wextra -masm=intel
QMAKE_CXXFLAGS  = $$QMAKE_CFLAGS -std=c++11 -fno-exceptions -fno-rtti
QMAKE_LFLAGS    = -T $$PWD/kernel.ld -ffreestanding -nostdlib
QMAKE_POST_LINK = $$OBJCOPY -O binary --set-section-flags .bss=alloc,load,contents $$OUT_PWD/kernel $$OUT_PWD/kernel.bin

LIBS += -lgcc

SOURCES += \
    entry.S \
    kernel.cpp

HEADERS += \
    kernel.hpp

OTHER_FILES += \
    kernel.ld \
