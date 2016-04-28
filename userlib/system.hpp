#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <stddef.h>
#include "console.hpp"

class System {
public:
    static constexpr auto CHILD_ANY = -1;
    static constexpr auto CHILD_ALL = -2;

    static void putchar(char c, Console::Color fg, Console::Color bg);
    static void print(const char* s, Console::Color fg, Console::Color bg);
    static void clear();
    static void sleep(size_t ms);
    static int exec(const char* executable);
    static void wait(int child);
    static void* increase(size_t pages);
    static void exit();
    static int pid();

private:
    static int syscall(int eax, int ebx, int ecx, int edx) asm("syscall");
};

#endif // SYSTEM_HPP
