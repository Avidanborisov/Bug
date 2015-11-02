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
    VirtualAllocator::init();
    Paging::init();
    PhysicalAllocator::finalize();

    Console::print("Paging initialized!\n");

    // make sure mapping works: map some virtual address to the video framebuffer
    Console::printf("%t\n", Paging::map(0x100000, 0xb8000));
    Console::printf("%t\n", Paging::map(0xff000, 0xb8000, 2));
    *(int *)0x100008 = 0xffffffff;
    Console::printf("%t\n", Paging::map(0x1000000, 0xb8000));
    *(int *)0x1000016 = 0xffffffff;
    Console::printf("%t\n", Paging::unmap(0x1000000));

    // page fault!
    // *(int *)0x1000000 = 0xffffffff;
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
