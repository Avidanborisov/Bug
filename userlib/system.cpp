#include "system.hpp"
#include "console.hpp"

void System::putchar(char c, Console::Color fg, Console::Color bg) {
    syscall(0, c, static_cast<int>(fg), static_cast<int>(bg));
}

void System::print(const char* s, Console::Color fg, Console::Color bg) {
    syscall(1, reinterpret_cast<int>(s), static_cast<int>(fg), static_cast<int>(bg));
}

void System::clear() {
    syscall(2, 0, 0, 0);
}

void System::sleep(size_t ms) {
    syscall(3, ms, 0, 0);
}

int System::exec(const char* executable) {
    return syscall(4, reinterpret_cast<int>(executable), 0, 0);
}

void System::wait(int child) {
    syscall(5, child, 0, 0);
}

void* System::increase(size_t pages) {
    return reinterpret_cast<void*>(syscall(6, pages, 0, 0));
}

void System::exit() {
    syscall(7, 0, 0, 0);
}

int System::pid() {
    return syscall(8, 0, 0, 0);
}
