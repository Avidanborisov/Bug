QMAKE_LINK = i686-elf-g++
QMAKE_CC   = i686-elf-gcc
QMAKE_CXX  = i686-elf-g++
OBJCOPY    = i686-elf-objcopy

QMAKE_CFLAGS       = -ffreestanding -Wall -Wextra -masm=intel
QMAKE_CXXFLAGS     = $$QMAKE_CFLAGS -std=c++14 -fno-exceptions -fno-rtti
QMAKE_CFLAGS_APP   =
QMAKE_CXXFLAGS_APP =
