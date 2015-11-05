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
    boot/entry.S \
    boot/bios.S \
    boot/e820.cpp \
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
    timer.cpp \
    memorymap.cpp \
    paging.cpp \
    physicalallocator.cpp \
    support/memops.cpp \
    virtualallocator.cpp

HEADERS += \
    boot/bios.hpp \
    boot/e820.hpp \
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
    timer.hpp \
    sort.hpp \
    memorymap.hpp \
    assert.hpp \
    math.hpp \
    paging.hpp \
    physicalallocator.hpp \
    containers/bitset.hpp \
    containers/array.hpp \
    support/memops.hpp \
    virtualallocator.hpp \
    containers/optional.hpp

# Global constructors support
#
# The following objects shall be included in the linking process to enable global
# constructors support. See http://wiki.osdev.org/Calling_Global_Constructors for reference
CRTI     = $$PWD/support/crti.S
CRTBEGIN = $$system($$QMAKE_CC $$QMAKE_CFLAGS -print-file-name=crtbegin.o)
CRTEND   = $$system($$QMAKE_CC $$QMAKE_CFLAGS -print-file-name=crtend.o)
CRTN     = $$PWD/support/crtn.S

QMAKE_LFLAGS += $$CRTI $$CRTBEGIN # These objects appear at the start of the link command
LIBS         += $$CRTEND $$CRTN   # These objects appear at the end of the link command

OTHER_FILES += \
    kernel.ld \
    support/crti.S \
    support/crtn.S
