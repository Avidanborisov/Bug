TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LINK = x86_64-elf-g++
QMAKE_CC   = x86_64-elf-gcc
QMAKE_CXX  = x86_64-elf-g++
OBJCOPY    = x86_64-elf-objcopy

QMAKE_CFLAGS    = -ffreestanding -Wall -Wextra -masm=intel
QMAKE_LFLAGS    = -T $$PWD/kernel.ld -ffreestanding -nostdlib
QMAKE_POST_LINK = $$OBJCOPY -O binary $$OUT_PWD/kernel $$OUT_PWD/kernel.bin

LIBS += -lgcc

SOURCES += \
    entry.S

OTHER_FILES += \
    kernel.ld
