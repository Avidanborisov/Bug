#ifndef X86_HPP
#define X86_HPP

#include <stdint.h>

class x86 {
public:
    static void out(uint16_t, uint8_t data) asm("x86_out");
    static uint8_t inb(uint16_t port)       asm("x86_inb");
    static uint16_t inw(uint16_t port)      asm("x86_inw");
};

#endif // X86_HPP
