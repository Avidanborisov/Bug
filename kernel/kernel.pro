TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

include(../build.pri)

QMAKE_LFLAGS    = -T $$PWD/kernel.ld -ffreestanding -nostdlib -lgcc
QMAKE_POST_LINK = $$OBJCOPY -O binary --set-section-flags .bss=alloc,load,contents $$OUT_PWD/kernel $$OUT_PWD/kernel.bin

# Import and link Bug's stdlib
LIBS += -L$$OUT_PWD/../stdlib/ -lstdlib
INCLUDEPATH += $$PWD/../stdlib
DEPENDPATH += $$PWD/../stdlib
PRE_TARGETDEPS += $$OUT_PWD/../stdlib/libstdlib.a

# Link all user program executables (data objects generated from elf executables)
LIBS += $$OUT_PWD/../programs/shell/shell.o

# Force link each time to get updated programs objects
forcelink.target = forcelink
forcelink.commands =
forcelink.depends =
QMAKE_EXTRA_TARGETS += forcelink
PRE_TARGETDEPS = forcelink

SOURCES += \
    boot/entry.S \
    boot/bios.S \
    boot/e820.cpp \
    x86.S \
    kernel.cpp \
    framebuffer.cpp \
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
    virtualallocator.cpp \
    tss.cpp \
    scheduler.cpp \
    task.S \
    programs.cpp \
    syscalls.cpp \
    terminal.cpp

HEADERS += \
    boot/bios.hpp \
    boot/e820.hpp \
    kernel.hpp \
    framebuffer.hpp \
    x86.hpp \
    gdt.hpp \
    idt.hpp \
    context.hpp \
    isr.hpp \
    irq.hpp \
    interrupts.hpp \
    timer.hpp \
    memorymap.hpp \
    assert.hpp \
    math.hpp \
    paging.hpp \
    physicalallocator.hpp \
    virtualallocator.hpp \
    tss.hpp \
    scheduler.hpp \
    programs.hpp \
    elf.hpp \
    task.h \
    syscalls.hpp \
    mappedpointer.hpp \
    terminal.hpp

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
    support/crtn.S \
