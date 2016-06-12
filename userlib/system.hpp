#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <stddef.h>
#include "console.hpp"
#include "datetime.hpp"

class System {
public:
    static constexpr auto CHILD_ANY = -1;
    static constexpr auto CHILD_ALL = -2;

    static void putchar(char c, Console::Color fg, Console::Color bg);
    static void print(const char* s, Console::Color fg, Console::Color bg);
    static void clear();

    static void sleep(size_t ms);
    static int exec(const char* executable, int tty = -1);
    static int wait(int child);
    static void exit();
    static int pid();
    static int tty();

    static void* increase(size_t pages);

    static size_t input(char* buffer, size_t max);
    static int read(const char* file, char* buffer, size_t count, size_t offset);
    static int write(const char* file, const char* buffer, size_t count, size_t offset);

    static void swd(const char* workingDirectory);
    static void swd(const String& workingDirectory);

    static DateTime date();
    static uint32_t random();
    static uint32_t random(uint32_t min, uint32_t max);

    static bool kill(int pid);

    static bool hasInput();
    static void setPosition(int x, int y);
    static void dontPrint(bool status);

    static String pwd();
    static Vector<String> files();
    static Vector<String> subdirs();

private:
    static int syscall(int eax, int ebx, int ecx, int edx) asm("syscall");
};

#endif // SYSTEM_HPP
