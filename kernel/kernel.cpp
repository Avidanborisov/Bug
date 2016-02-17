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
#include "string.hpp"
#include "utility.hpp"

void Kernel::main() {
    Timer::disable();

    Console::init();
    GDT::init();
    Interrupts::init();

    MemoryMap::init();
    PhysicalAllocator::init();
    Paging::init();

    String s = "hello";
    Console::print(s + '\n');
    s += " world! yeah!!!!!!";
    Console::printf(Console::Color::RED, "%s\n", s);
    Console::printf("%s\n", "abcd: " + s + '%');

    auto copy = Util::move(s);
    Console::printf("%s\n", copy);
    Console::printf("%s\n", s);
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
