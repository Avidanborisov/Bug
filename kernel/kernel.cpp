#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "interrupts.hpp"
#include "timer.hpp"
#include "memorymap.hpp"
#include "boot/e820.hpp"
#include "array.hpp"

void Kernel::main() {
    Timer::disable();

    Console::init();
    GDT::init();
    Interrupts::init();

    E820::sanitizeMap();
}
