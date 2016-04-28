TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

include(../build.pri)

QMAKE_LFLAGS    = -T $$PWD/programs.ld -ffreestanding -nostdlib -lgcc

# Import and link Bug's userlib
LIBS += -L$$OUT_PWD/../../userlib/ -luserlib
INCLUDEPATH += $$PWD/../userlib
DEPENDPATH += $$PWD/../userlib
PRE_TARGETDEPS += $$OUT_PWD/../../userlib/libuserlib.a

# Import and link Bug's stdlib
LIBS += -L$$OUT_PWD/../../stdlib/ -lstdlib
INCLUDEPATH += $$PWD/../stdlib
DEPENDPATH += $$PWD/../stdlib
PRE_TARGETDEPS += $$OUT_PWD/../../stdlib/libstdlib.a

EXE = $$TARGET
BIN = $${EXE}.o
QMAKE_POST_LINK = cd $$OUT_PWD; $$OBJCOPY -B i386 -I binary -O elf32-i386 $$EXE $$BIN

OTHER_FILES = programs.ld
