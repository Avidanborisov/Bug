#ifndef PAGING_HPP
#define PAGING_HPP

#include <stdint.h>
#include "math.hpp"

class Paging {
public:
    static constexpr uint32_t PAGE_SIZE = 4096;

    constexpr static uint32_t alignDown(uint32_t address) {
        return Math::roundDown(address, PAGE_SIZE);
    }

    template<class T>
    constexpr static uint32_t alignDown(T* address) {
        return alignDown((uint32_t)address);
    }

    constexpr static uint32_t alignUp(uint32_t address) {
        return Math::roundUp(address, PAGE_SIZE);
    }

    template<class T>
    constexpr static uint32_t alignUp(T* address) {
        return alignUp((uint32_t)address);
    }
};

#endif // PAGING_HPP
