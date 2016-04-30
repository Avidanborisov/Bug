#ifndef PHYSICALALLOCATOR_HPP
#define PHYSICALALLOCATOR_HPP

#include <stdint.h>
#include <stddef.h>
#include "paging.hpp"
#include "bitset.hpp"

class PhysicalAllocator {
public:
    static uint32_t reserve(size_t pages);
    static uint32_t getKernelEnd();

    static void init();

    static uint32_t allocate(size_t pages);
    static void free(uint32_t address, size_t pages);

    template<class T>
    static T allocate(size_t pages) {
        return reinterpret_cast<T>(allocate(pages));
    }

    template<class T>
    static void free(T* address, size_t pages) {
        free(reinterpret_cast<uint32_t>(address), pages);
    }

    static size_t used();
    static size_t available();

private:
    static uint32_t kernelEnd;
    static bool initialized;

    static Bitset<> memory;
    static size_t usedPages;
};

#endif // PHYSICALALLOCATOR_HPP
