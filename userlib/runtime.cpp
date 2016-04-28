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

