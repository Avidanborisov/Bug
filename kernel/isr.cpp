#include "isr.hpp"
#include "idt.hpp"
#include "console.hpp"
#include "syscalls.hpp"
#include "kernel.hpp"
#include "scheduler.hpp"
#include "terminal.hpp"
#include "x86.hpp"

extern "C" void  isr0(void);
extern "C" void  isr1(void);
extern "C" void  isr2(void);
extern "C" void  isr3(void);
extern "C" void  isr4(void);
extern "C" void  isr5(void);
extern "C" void  isr6(void);
extern "C" void  isr7(void);
extern "C" void  isr8(void);
extern "C" void  isr9(void);
extern "C" void isr10(void);
extern "C" void isr11(void);
extern "C" void isr12(void);
extern "C" void isr13(void);
extern "C" void isr14(void);
extern "C" void isr15(void);
extern "C" void isr16(void);
extern "C" void isr17(void);
extern "C" void isr18(void);
extern "C" void isr19(void);
extern "C" void isr20(void);
extern "C" void isr21(void);
extern "C" void isr22(void);
extern "C" void isr23(void);
extern "C" void isr24(void);
extern "C" void isr25(void);
extern "C" void isr26(void);
extern "C" void isr27(void);
extern "C" void isr28(void);
extern "C" void isr29(void);
extern "C" void isr30(void);
extern "C" void isr31(void);

extern "C" void isr80(void);

void ISR::init() {
    IDT::setEntry(0 , isr0);
    IDT::setEntry(1 , isr1);
    IDT::setEntry(2 , isr2);
    IDT::setEntry(3 , isr3);
    IDT::setEntry(4 , isr4);
    IDT::setEntry(5 , isr5);
    IDT::setEntry(6 , isr6);
    IDT::setEntry(7 , isr7);
    IDT::setEntry(8 , isr8);
    IDT::setEntry(9 , isr9);
    IDT::setEntry(10, isr10);
    IDT::setEntry(11, isr11);
    IDT::setEntry(12, isr12);
    IDT::setEntry(13, isr13);
    IDT::setEntry(14, isr14);
    IDT::setEntry(15, isr15);
    IDT::setEntry(16, isr16);
    IDT::setEntry(17, isr17);
    IDT::setEntry(18, isr18);
    IDT::setEntry(19, isr19);
    IDT::setEntry(10, isr20);
    IDT::setEntry(21, isr21);
    IDT::setEntry(22, isr22);
    IDT::setEntry(23, isr23);
    IDT::setEntry(24, isr24);
    IDT::setEntry(25, isr25);
    IDT::setEntry(26, isr26);
    IDT::setEntry(27, isr27);
    IDT::setEntry(28, isr28);
    IDT::setEntry(29, isr29);
    IDT::setEntry(30, isr30);
    IDT::setEntry(31, isr31);

    IDT::setEntry(80, isr80, 3); // syscall handler
}

const char* ISR::exceptions[NUM_EXCEPTIONS] {
    "Division by zero",
    "Debugger",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bounds",
    "Invalid Opcode",
    "Coprocessor not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid Task State Segment",
    "Segment not present",
    "Stack Fault",
    "General protection fault",
    "Page Fault",
    "Reserved",
    "Math Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void ISR::commonHandler(Context::Registers& regs) {
    if (regs.intNum == SYSCALL_HANDLER) {
        Syscalls::handle(regs);
        return;
    }

    if (regs.intNum > NUM_EXCEPTIONS) {
        Kernel::panic("Unhandled interrupt: %d\n", regs.intNum);
    }

    auto ex = regs.intNum;

    if (Scheduler::started()) {
        auto pid = Scheduler::current();
        Terminal::setActive(Scheduler::tty(pid));
        Console::printf(Console::Color::BLUE, "Exception %u (%s) occured in task %d\n", ex, exceptions[ex], pid);
    } else {
        Console::printf(Console::Color::RED,  "Exception %u (%s) occured\n", ex, exceptions[ex]);
    }

    Console::printf("error code = 0x%x\n", regs.errorCode);
    Console::printf("eip        = 0x%x\n", regs.eip);
    Console::printf("esp        = 0x%x\n", regs.esp);
    Console::printf("eflags     = 0x%x\n", regs.eflags);
    Console::printf("cs         = 0x%x\n", regs.cs);
    Console::printf("ss         = 0x%x\n", regs.ss);
    Console::printf("cr2        = 0x%x\n", x86::regs::cr2());
    Console::printf("cr3        = 0x%x\n", x86::regs::cr3());

    // page fault
    if (ex == 14) {
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
    }

    if (Scheduler::started()) {
        Console::printf(Console::Color::RED, "Aborting task!\n");
        Scheduler::exit();
    } else {
        Kernel::panic("Exception occured");
    }
}
