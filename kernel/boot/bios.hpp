#ifndef BIOS_HPP
#define BIOS_HPP

#include <stdint.h>

class BIOS {
public:
    struct RegPack {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
        uint32_t edi;
    };

    static RegPack interrupt(uint8_t n, RegPack input) asm("BIOS_interrupt");
};

#endif // BIOS_HPP
