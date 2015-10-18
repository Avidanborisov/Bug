#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "interrupts.hpp"
#include "timer.hpp"

void Kernel::main() {
    Timer::disable();

    Console::init();
    GDT::init();
    Interrupts::init();
}
