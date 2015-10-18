#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "interrupts.hpp"
#include "timer.hpp"
#include "isr.hpp"
#include "e820.hpp"

void Kernel::main() {
    Timer::disable();

    Console::init();
    GDT::init();
    Interrupts::init();

    for (int i = 0; i < E820::Entry::count; ++i) {
        auto& e = E820::entries[i];
        Console::printf("%x%.08x %x%.08x %x %x\n", e.baseHigh, e.baseLow, e.lengthHigh, e.lengthLow, e.acpi, e.type);
    }
}
