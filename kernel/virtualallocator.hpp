#ifndef VIRTUALALLOCATOR_HPP
#define VIRTUALALLOCATOR_HPP

#include <stdint.h>
#include <stddef.h>
#include "paging.hpp"
#include "containers/bitset.hpp"

class VirtualAllocator {
public:
    static void init();
    static void exclude(uint32_t base, size_t length);

    static uint32_t allocate(size_t pages);
    static void free(uint32_t address, size_t pages);

private:
    static Bitset<> pages;
};

#endif // VIRTUALALLOCATOR_HPP
