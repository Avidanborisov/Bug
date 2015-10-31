#ifndef PHYSICALALLOCATOR_HPP
#define PHYSICALALLOCATOR_HPP

#include <stdint.h>
#include <stddef.h>
#include "paging.hpp"
#include "containers/bitset.hpp"

class PhysicalAllocator {
public:
    static constexpr auto BLOCK_SIZE = Paging::PAGE_SIZE;

    static uint32_t reserve(size_t blocks);
    static uint32_t getKernelEnd();

    static void init();

    static uint32_t allocate(size_t blocks);
    static void free(uint32_t address, size_t blocks);

private:
    static uint32_t kernelEnd;
    static bool initialized;

    class AddressBlockMap {
    public:
        static constexpr auto BLOCK_SIZE = PhysicalAllocator::BLOCK_SIZE;

        static void init(uint32_t endAddress);
        static uint32_t findFreeBlocks(size_t blocks);

        static void mark(uint32_t address, size_t blocks);
        static void free(uint32_t address, size_t blocks);

    private:
        static Bitset<> memory;
        static uint32_t end;
    };
};

#endif // PHYSICALALLOCATOR_HPP
