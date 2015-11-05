#ifndef VIRTUALALLOCATOR_HPP
#define VIRTUALALLOCATOR_HPP

#include <stdint.h>
#include <stddef.h>
#include "paging.hpp"
#include "containers/bitset.hpp"

class VirtualAllocator {
public:
    static Optional<uint32_t> allocate(size_t pages);

    template<class T>
    static T allocate(size_t pages) {
        auto address = allocate(pages);
        if (!address)
            return nullptr;

        return reinterpret_cast<T>(*address);
    }

    static bool free(uint32_t address, size_t pages);

    template<class T>
    static bool free(T* address, size_t pages) {
        return free(reinterpret_cast<uint32_t>(address), pages);
    }
};

#endif // VIRTUALALLOCATOR_HPP
