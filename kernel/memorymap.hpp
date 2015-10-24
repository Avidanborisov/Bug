#ifndef MEMORYMAP_HPP
#define MEMORYMAP_HPP

#include <stdint.h>
#include "array.hpp"
#include "boot/e820.hpp"

class MemoryMap {
public:
    static void init();

    struct Entry {
        uint32_t base;
        uint32_t length;
        bool available;

        Entry() = default;
        Entry(uint64_t base, uint64_t length, bool available);
        Entry(const E820::Entry& e);
    };

    static Array<Entry, E820::MAX_ENTRIES * 2> entries; // 2x to save space for unlisted regions
};

#endif // MEMORYMAP_HPP
