#include "system.hpp"

void* allocator(size_t blocks) {
    return System::increase(blocks);
}

void clear() {
    return System::clear();
}

void putchar(char c, Console::Color fg, Console::Color bg) {
    return System::putchar(c, fg, bg);
}

void print(const char* s, Console::Color fg, Console::Color bg) {
    return System::print(s, fg, bg);
}

size_t input(char* buffer, size_t max) {
    return System::input(buffer, max);
}

int read(const char* file, char* buffer, size_t count, size_t offset = 0) {
    return System::read(file, buffer, count, offset);
}

int write(const char* file, const char* buffer, size_t count, size_t offset = 0) {
    return System::write(file, buffer, count, offset);
}

extern void main();

extern "C" void abort(const char* msg, ...) {
    va_list ap;
    va_start(ap, msg);

    Console::vprintf(Console::Color::LIGHT_GREY, msg, ap);
    Console::printf(Console::Color::RED, "Task %d aborted!\n", System::pid());
    Console::print('\n');

    System::exit();
}

extern "C" void entry() {
    main();
    System::exit();
}

