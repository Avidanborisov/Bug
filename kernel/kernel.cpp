#include <stdint.h>
#include "kernel.hpp"
#include "console.hpp"
#include "x86.hpp"

void Kernel::main() {
    Console::init();

    x86::regs::cs = 0x08;
    x86::regs::ds = 0x10;

    Console::printf("%u %u %b", x86::regs::cs(), x86::regs::ds(), x86::regs::cr0());
}
