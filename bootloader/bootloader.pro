TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LINK = i686-elf-g++
QMAKE_CC   = i686-elf-gcc
QMAKE_CXX  = i686-elf-g++

QMAKE_CFLAGS = -ffreestanding -Wall -Wextra -masm=intel
QMAKE_LFLAGS = -T $$PWD/bootloader.ld -ffreestanding -nostdlib
LIBS += -lgcc

SOURCES += \
    boot.S

OTHER_FILES += \
    bootloader.ld
