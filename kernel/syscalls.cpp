#include "syscalls.hpp"
#include "kernel.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "terminal.hpp"
#include "filesystem.hpp"
#include "rtc.hpp"

ISR::Handler* Syscalls::handlers[] = {
    Syscalls::putchar,
    Syscalls::print,
    Syscalls::clear,
    Syscalls::sleep,
    Syscalls::exec,
    Syscalls::wait,
    Syscalls::increase,
    Syscalls::exit,
    Syscalls::pid,
    Syscalls::input,
    Syscalls::read,
    Syscalls::write,
    Syscalls::swd,
    Syscalls::date,
    Syscalls::kill,
    Syscalls::hasInput,
    Syscalls::setPosition,
    Syscalls::dontPrint
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

    auto tty = Scheduler::tty(Scheduler::current());
    Terminal::get(tty).putchar(c, fg, bg);
}

void Syscalls::print(Context::Registers& regs) {
    auto str = reinterpret_cast<const char*>(regs.ebx);
    auto fg = static_cast<Console::Color>(regs.ecx);
    auto bg = static_cast<Console::Color>(regs.edx);

    auto tty = Scheduler::tty(Scheduler::current());
    Terminal::get(tty).print(str, fg, bg);
}

void Syscalls::clear(Context::Registers&) {
    auto tty = Scheduler::tty(Scheduler::current());
    Terminal::get(tty).clear();
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
    regs.eax = Scheduler::wait(child);
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

void Syscalls::input(Context::Registers& regs) {
    auto buffer = reinterpret_cast<char*>(regs.ebx);
    size_t max = regs.ecx;

    auto tty = Scheduler::tty(Scheduler::current());
    regs.eax = Terminal::get(tty).read(buffer, max);
}

void Syscalls::read(Context::Registers& regs) {
    auto file   = reinterpret_cast<char*>(regs.ebx);
    auto buffer = reinterpret_cast<char*>(regs.ecx);
    auto params = reinterpret_cast<size_t*>(regs.edx);

    auto count  = params[0];
    auto offset = params[1];

    regs.eax = Filesystem::read(file, buffer, count, offset);
}

void Syscalls::write(Context::Registers& regs) {
    auto file   = reinterpret_cast<char*>(regs.ebx);
    auto buffer = reinterpret_cast<char*>(regs.ecx);
    auto params = reinterpret_cast<size_t*>(regs.edx);

    auto count  = params[0];
    auto offset = params[1];

    regs.eax = Filesystem::write(file, buffer, count, offset);
}

void Syscalls::swd(Context::Registers& regs) {
    auto buffer = reinterpret_cast<const char*>(regs.ebx);
    auto wd = String(buffer).split('/');
    Scheduler::setWorkingDirectory(wd);
}

void Syscalls::date(Context::Registers& regs) {
    auto datetime = reinterpret_cast<DateTime*>(regs.ebx);
    *datetime = RTC::get();
}

void Syscalls::kill(Context::Registers& regs) {
    auto pid = regs.ebx;

    if (Scheduler::isKernelTask(pid)) {
        regs.eax = 0;
        return;
    }

    Scheduler::kill(pid);
    regs.eax = 1;
}

void Syscalls::hasInput(Context::Registers& regs) {
    auto tty = Scheduler::tty(Scheduler::current());
    regs.eax = Terminal::get(tty).hasInput();
}

void Syscalls::setPosition(Context::Registers& regs) {
    int x = regs.ebx;
    int y = regs.ecx;
    auto tty = Scheduler::tty(Scheduler::current());
    Terminal::get(tty).setPosition(x, y);
}

void Syscalls::dontPrint(Context::Registers& regs) {
    bool status = regs.ebx;
    auto tty = Scheduler::tty(Scheduler::current());
    Terminal::get(tty).dontPrint(status);
}
