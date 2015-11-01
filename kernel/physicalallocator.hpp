#ifndef PHYSICALALLOCATOR_HPP
#define PHYSICALALLOCATOR_HPP

#include <stdint.h>
#include <stddef.h>
#include "paging.hpp"
#include "containers/bitset.hpp"

class PhysicalAllocator {
public:
    static uint32_t reserve(size_t pages);
    static uint32_t getKernelEnd();

    static void init();
    static void finalize();

    static uint32_t allocate(size_t blocks);
    static void free(uint32_t address, size_t blocks);

private:
    static uint32_t kernelEnd;
    static bool finalized;

    static Bitset<> memory;
};

#endif // PHYSICALALLOCATOR_HPP
