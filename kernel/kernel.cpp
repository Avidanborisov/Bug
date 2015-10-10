#include <stdint.h>
#include "kernel.hpp"
#include "console.hpp"
#include "x86.hpp"
#include "gdt.hpp"

void Kernel::main() {
    Console::init();
    GDT::init();

    Console::printf("GDT loaded (cs=0x%x ds=0x%x)\n", x86::regs::cs(), x86::regs::ds());
}
