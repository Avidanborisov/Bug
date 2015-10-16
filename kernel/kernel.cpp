#include <stdint.h>
#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "interrupts.hpp"

void Kernel::main() {
    Console::init();
    GDT::init();
    Interrupts::init();

    asm volatile("int 9");
}
