TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LINK = i686-elf-g++
QMAKE_CC   = i686-elf-gcc
QMAKE_CXX  = i686-elf-g++
OBJCOPY    = i686-elf-objcopy

QMAKE_CFLAGS    = -ffreestanding
QMAKE_LFLAGS    = -T $$PWD/bootloader.ld -ffreestanding -nostdlib
QMAKE_POST_LINK = $$OBJCOPY -O binary $$OUT_PWD/bootloader $$OUT_PWD/bootloader.bin

SOURCES += \
    boot.S \
    io.S \
    disk.S \
    fat12.S

OTHER_FILES += \
    bootloader.ld \
