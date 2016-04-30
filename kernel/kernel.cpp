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
#include "keyboard.hpp"
#include "filesystem.hpp"
#include "sysfs.hpp"
#include "file.hpp"

void Kernel::main() {
    Timer::disable();

    Console::clear();

    GDT::init();
    Interrupts::init();

    MemoryMap::init();
    PhysicalAllocator::init();
    Paging::init();

    Filesystem::init();
    Keyboard::install();

    Sysfs::set("/sys/mem", []() {
        return "Total physical memory used: " + String::fromInt(PhysicalAllocator::used()) + " bytes\n" +
               "Physical memory available:  " + String::fromInt(PhysicalAllocator::available()) + " bytes\n";
    });

    Scheduler::start([] {
        Vector<int> shells;

        for (size_t i = 0; i < Terminal::NUM_TERMINALS; ++i) {
            Terminal::setActive(i);
            shells.add(Scheduler::exec("shell"));
        }

        Terminal::setActive(0);

        // keep execting the shells forever
//        while (true) {
//            int exitedShell = Scheduler::wait(Scheduler::CHILD_ANY);

//            auto curr = Terminal::getActiveTTY();
//            for (size_t i = 0; i < shells.size(); ++i) {
//                if (shells[i] == exitedShell) {
//                    Terminal::setActive(i);
//                    shells[i] = Scheduler::exec("shell");
//                    Terminal::setActive(curr);
//                }
//            }
//        }

        Scheduler::wait(Scheduler::CHILD_ALL);
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
