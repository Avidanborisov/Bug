#ifndef SYSCALLS_HPP
#define SYSCALLS_HPP

#include <stddef.h>
#include "context.hpp"
#include "isr.hpp"

class Syscalls {
public:
    static void handle(Context::Registers& regs);

private:
    static ISR::Handler* handlers[];
    static const size_t NUM_SYSCALLS;

    static void putchar(Context::Registers& regs);
    static void print(Context::Registers& regs);
    static void clear(Context::Registers& regs);
    static void sleep(Context::Registers& regs);
    static void exec(Context::Registers& regs);
    static void wait(Context::Registers& regs);
    static void increase(Context::Registers& regs);
    static void exit(Context::Registers& regs);
    static void pid(Context::Registers& regs);
    static void input(Context::Registers& regs);
    static void read(Context::Registers& regs);
    static void write(Context::Registers& regs);
    static void swd(Context::Registers& regs);
    static void date(Context::Registers& regs);
    static void kill(Context::Registers& regs);
    static void hasInput(Context::Registers& regs);
    static void setPosition(Context::Registers& regs);
    static void dontPrint(Context::Registers& regs);
};

#endif // SYSCALLS_HPP
