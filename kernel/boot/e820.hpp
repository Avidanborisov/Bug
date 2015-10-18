#ifndef E820_HPP
#define E820_HPP

#include <stdint.h>

class E820 {
public:
    static constexpr auto MAX_ENTRIES = 20;

    static struct [[gnu::packed]] Entry {
        uint32_t baseLow;
        uint32_t baseHigh;
        uint32_t lengthLow;
        uint32_t lengthHigh;
        uint16_t type;
        uint16_t acpi;

        static uint16_t count;

        enum Type {
            AVAILABLE = 1,
            RESERVED  = 2,
        };
    } entries[];

    static_assert(sizeof(Entry) == 20, "E820 entry is 20 bytes");

    static void detectMemory() asm("detectMemory"); // CALL ONLY FROM REAL MODE!

private:
    static constexpr auto SMAP_SIGNATURE = 0x534d4150; // 'SMAP'
};

#endif // E820_HPP
