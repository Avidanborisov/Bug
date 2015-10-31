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
        Entry(uint64_t start, uint64_t end, bool available); // Entry for address range [start, end)
        Entry(const E820::Entry& e);
    };

    static uint32_t getEnd();

    using HoleList = Array<Entry, E820::MAX_ENTRIES>;
    static const HoleList& getHoles();

private:
    static uint32_t start;
    static uint32_t end;
    static HoleList holes;
};

#endif // MEMORYMAP_HPP
