#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "interrupts.hpp"
#include "timer.hpp"
#include "memorymap.hpp"
#include "x86.hpp"
#include "physicalallocator.hpp"
#include "paging.hpp"
#include "string.hpp"
#include "utility.hpp"
#include "heap.hpp"
#include "scheduler.hpp"
#include "isr.hpp"
#include "irq.hpp"
#include "terminal.hpp"

void Kernel::main() {
    Timer::disable();

    Console::clear();
    Console::print("                            Welcome to Bug OS!\n", Console::Color::CYAN);

    GDT::init();
    Interrupts::init();

    MemoryMap::init();
    PhysicalAllocator::init();
    Paging::init();

    Console::printf("%x\n", PhysicalAllocator::getKernelEnd());

    ISR::handle(13, [](Context::Registers& regs) {
        Console::printf("Instruction address: %x\n", regs.eip);
        Console::printf("Selector: %b\n", regs.errorCode);
        panic("GP");
    });

    ISR::handle(14, [](Context::Registers& regs) {
        static const char* flagsMeanings[][2] = {
            { "P    = 0: The fault was caused by a non-present page.",
              "P    = 1: The fault was caused by a page-level protection violation." },
            { "W/R  = 0: The access causing the fault was a read.",
              "W/R  = 1: The access causing the fault was a write." },
            { "U/S  = 0: A supervisor-mode access caused the fault.",
              "U/S  = 1: A user-mode access caused the fault." },
            { "RSVD = 0: The fault was not caused by reserved bit violation",
              "RSVD = 1: The fault was caused by a reserved bit set to 1 in some paging-structure entry." },
            { "I/D  = 0: The fault was not caused by an instruction fetch.",
              "I/D  = 1: The fault was caused by an instruction fetch." }
        };

        const size_t NUM_FLAGS = sizeof flagsMeanings / sizeof *flagsMeanings;

        uint32_t faultAddr = x86::regs::cr2();
        Console::printf("Page Fault at %x!\n", faultAddr);

        for (size_t i = 0; i < NUM_FLAGS; ++i) {
            bool status = (regs.errorCode & (1 << i));
            Console::printf("\t%s\n", flagsMeanings[i][status]);
        }

        panic("Page Fault (instruction: %x)", regs.eip);
    });

    Scheduler::start([] {
        int shell = Scheduler::exec("shell");
        Scheduler::wait(shell);

        for (int i = 1; i < 10; ++i) {
            Terminal::setActive(i % 2);
            Console::printf("wasap %d\n", i);
            Scheduler::sleep(Scheduler::current(), 2000);
        }

        Scheduler::exit();
    });
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
