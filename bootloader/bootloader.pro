TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

include(../build.pri)

QMAKE_LFLAGS     = -T $$PWD/bootloader.ld -ffreestanding -nostdlib
QMAKE_POST_LINK  = $$OBJCOPY -O binary $$OUT_PWD/bootloader $$OUT_PWD/bootloader.bin

SOURCES += \
    boot.S \
    io.S \
    disk.S \
    fat12.S

OTHER_FILES += \
    bootloader.ld \
