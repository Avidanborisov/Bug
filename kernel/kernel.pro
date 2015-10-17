TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LINK = i686-elf-g++
QMAKE_CC   = i686-elf-gcc
QMAKE_CXX  = i686-elf-g++
OBJCOPY    = i686-elf-objcopy

QMAKE_CFLAGS    = -ffreestanding -Wall -Wextra -masm=intel
QMAKE_CXXFLAGS  = $$QMAKE_CFLAGS -std=c++14 -fno-exceptions -fno-rtti
QMAKE_LFLAGS    = -T $$PWD/kernel.ld -ffreestanding -nostdlib -lgcc
QMAKE_POST_LINK = $$OBJCOPY -O binary --set-section-flags .bss=alloc,load,contents $$OUT_PWD/kernel $$OUT_PWD/kernel.bin

SOURCES += \
    entry.S \
    x86.S \
    kernel.cpp \
    framebuffer.cpp \
    console.cpp \
    gdt.cpp \
    idt.cpp \
    context.S \
    isr.cpp \
    isrs.S \
    irq.cpp \
    irqs.S \
    interrupts.cpp \
    timer.cpp

HEADERS += \
    kernel.hpp \
    framebuffer.hpp \
    x86.hpp \
    console.hpp \
    gdt.hpp \
    idt.hpp \
    context.hpp \
    isr.hpp \
    irq.hpp \
    interrupts.hpp \
    timer.hpp

# Global constructors support
#
# The following objects shall be included in the linking process to enable global
# constructors support. See http://wiki.osdev.org/Calling_Global_Constructors for reference
CRTI     = $$PWD/crti.S
CRTBEGIN = $$system($$QMAKE_CC $$QMAKE_CFLAGS -print-file-name=crtbegin.o)
CRTEND   = $$system($$QMAKE_CC $$QMAKE_CFLAGS -print-file-name=crtend.o)
CRTN     = $$PWD/crtn.S

QMAKE_LFLAGS += $$CRTI $$CRTBEGIN # These objects appear at the start of the link command
LIBS         += $$CRTEND $$CRTN   # These objects appear at the end of the link command

OTHER_FILES += \
    kernel.ld \
    crti.S \
    crtn.S \
