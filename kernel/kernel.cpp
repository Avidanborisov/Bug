#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "interrupts.hpp"
#include "timer.hpp"
#include "memorymap.hpp"
#include "x86.hpp"
#include "physicalallocator.hpp"
#include "virtualallocator.hpp"
#include "paging.hpp"

void Kernel::main() {
    Timer::disable();

    Console::init();
    GDT::init();
    Interrupts::init();

    MemoryMap::init();
    PhysicalAllocator::init();
    Paging::init();

    auto n = new int;
    Console::printf("%p\n", n);
    delete n;
}

void Kernel::panic(const char* msg, ...) {
    va_list ap;
    va_start(ap, msg);

    Console::print("\n*** KERNEL PANIC ***\n", Console::Color::RED);
    Console::vprintf(Console::Color::LIGHT_GREY, msg, ap);
    Console::print('\n');

    x86::cli();
    while (true) { }
}
