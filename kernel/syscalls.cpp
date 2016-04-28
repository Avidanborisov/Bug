#include "syscalls.hpp"
#include "kernel.hpp"
#include "console.hpp"
#include "scheduler.hpp"

ISR::Handler* Syscalls::handlers[] = {
    Syscalls::putchar,
    Syscalls::print,
    Syscalls::clear,
    Syscalls::sleep,
    Syscalls::exec,
    Syscalls::wait,
    Syscalls::increase,
    Syscalls::exit
};

const size_t Syscalls::NUM_SYSCALLS = sizeof(Syscalls::handlers) / sizeof(Syscalls::handlers[0]);

void Syscalls::handle(Context::Registers& regs) {
    if (regs.eax > NUM_SYSCALLS) {
        Kernel::panic("Invalid syscall %d\n", regs.eax);
    }

    handlers[regs.eax](regs);
}

void Syscalls::putchar(Context::Registers& regs) {
    char c = regs.ebx;
    auto fg = static_cast<Console::Color>(regs.ecx);
    auto bg = static_cast<Console::Color>(regs.edx);

    Console::print(c, fg, bg);
}

void Syscalls::print(Context::Registers& regs) {
    auto str = reinterpret_cast<const char*>(regs.ebx);
    auto fg = static_cast<Console::Color>(regs.ecx);
    auto bg = static_cast<Console::Color>(regs.edx);

    Console::print(str, fg, bg);
}

void Syscalls::clear(Context::Registers&) {
    Console::clear();
}

void Syscalls::sleep(Context::Registers& regs) {
    size_t ms = regs.ebx;
    Scheduler::sleep(ms);
}

void Syscalls::exec(Context::Registers& regs) {
    auto executable = reinterpret_cast<const char*>(regs.ebx);
    regs.eax = Scheduler::exec(executable);
}

void Syscalls::wait(Context::Registers& regs) {
    auto child = regs.ebx;
    Scheduler::wait(child);
}

void Syscalls::increase(Context::Registers& regs) {
    auto pages = regs.ebx;
    regs.eax = reinterpret_cast<uint32_t>(Scheduler::increase(pages));
}

void Syscalls::exit(Context::Registers&) {
    Scheduler::exit();
}

void Syscalls::pid(Context::Registers& regs) {
    regs.eax = Scheduler::current();
}
