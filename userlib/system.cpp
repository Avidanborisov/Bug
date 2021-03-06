#include "system.hpp"
#include "console.hpp"
#include "file.hpp"

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

int System::exec(const char* executable, int tty) {
    return syscall(4, reinterpret_cast<int>(executable), reinterpret_cast<int>(tty), 0);
}

int System::wait(int child) {
    return syscall(5, child, 0, 0);
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

int System::tty() {
    return syscall(9, 0, 0, 0);
}

size_t System::input(char* buffer, size_t max) {
    return syscall(10, reinterpret_cast<int>(buffer), static_cast<int>(max), 0);
}

int System::read(const char* file, char* buffer, size_t count, size_t offset) {
    size_t params[] = { count, offset };
    return syscall(11, reinterpret_cast<int>(file), reinterpret_cast<int>(buffer), reinterpret_cast<int>(params));
}

int System::write(const char* file, const char* buffer, size_t count, size_t offset) {
    size_t params[] = { count, offset };
    return syscall(12, reinterpret_cast<int>(file), reinterpret_cast<int>(buffer), reinterpret_cast<int>(params));
}

void System::swd(const char* workingDirectory) {
    syscall(13, reinterpret_cast<int>(workingDirectory), 0, 0);
}

void System::swd(const String& workingDirectory) {
    swd(workingDirectory.data());
}

DateTime System::date() {
    DateTime datetime;
    syscall(14, reinterpret_cast<int>(&datetime), 0, 0);
    return datetime;
}

uint32_t System::random() {
    static uint32_t next = [] {
        DateTime dt = date();
        return ((((dt.month * 31 + dt.day) * 24 + dt.hour) * 60 + dt.minutes) * 60 + dt.seconds) * 60;
    }();

    next = next * 1103515245 + 12345;
    return next;
}

uint32_t System::random(uint32_t min, uint32_t max) {
    return (random() % (max - min + 1) + min);
}

bool System::kill(int pid) {
    return syscall(15, pid, 0, 0);
}

bool System::hasInput() {
    return syscall(16, 0, 0, 0);
}

void System::setPosition(int x, int y) {
    syscall(17, x, y, 0);
}

void System::dontPrint(bool status) {
    syscall(18, status, 0, 0);
}

String System::pwd() {
    return File::read("/task/wd");
}

Vector<String> System::files() {
    return String(File::read("/task/files")).split();
}

Vector<String> System::subdirs() {
    return String(File::read("/task/dirs")).split();
}
