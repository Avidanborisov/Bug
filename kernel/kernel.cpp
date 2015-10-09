#include <stdint.h>
#include "kernel.hpp"
#include "console.hpp"

void Kernel::main() {
    Console::init();
    Console::print("                            Welcome to Bug OS!\n", Console::Color::CYAN);

    Console::printf(Console::Color::GREEN, "%% %c %s %t %.7u %.05x %.08d %b 0x%p\n", 'A', "Hello", false, 1234, 0xABC, -100, 127, (const void*)main);
}
