#ifndef E820_HPP
#define E820_HPP

#include <stdint.h>
#include "array.hpp"

class E820 {
public:
    static constexpr auto MAX_ENTRIES = 20;

    struct [[gnu::packed]] Entry {
        uint64_t base;
        uint64_t length;
        uint16_t type;
        uint16_t acpi;

        enum Type {
            AVAILABLE        = 1,
            RESERVED         = 2,
            ACPI_RECLAIMABLE = 3,
            ACPI_NVS         = 4,
            BAD_MEMORY       = 5,
        };

        constexpr bool isAvailable() const {
            return type == AVAILABLE;
        }
    };

    static_assert(sizeof(Entry) == 20, "E820 entry is 20 bytes");
    static Array<Entry, MAX_ENTRIES> map;

    static void detectMemory() asm("detectMemory"); // CALL ONLY FROM REAL MODE!
    static void sanitizeMap();

private:
    static Entry entries[];
    static uint16_t count;

    static constexpr auto SMAP_SIGNATURE = 0x534d4150; // 'SMAP'
};

#endif // E820_HPP
