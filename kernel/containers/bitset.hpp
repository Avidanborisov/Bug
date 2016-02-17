#ifndef BITSET_HPP
#define BITSET_HPP

#include <stdint.h>
#include <stddef.h>
#include "support/memops.hpp"
#include "assert.hpp"
#include "math.hpp"
#include "optional.hpp"

template<class Unit = uint32_t>
class Bitset {
public:
    static constexpr size_t BITS_PER_UNIT = sizeof(Unit) * 8;

    template<class StaticAllocator>
    void init(size_t size, size_t allocBlockSize, StaticAllocator alloc) {
        assert(size != 0);
        units = Math::divRoundUp(size, BITS_PER_UNIT);
        auto blocks = Math::divRoundUp(units * sizeof(Unit), allocBlockSize);

        arr = reinterpret_cast<Unit*>(alloc(blocks));
        clear();
    }

    size_t size() const {
        return units * BITS_PER_UNIT;
    }

    // set n bits
    void set(size_t index, size_t bits = 1) {
        assert(index + bits <= size());

        for (size_t bit = index; bit < index + bits; ++bit) {
            arr[bit / BITS_PER_UNIT] |= 1 << (bit % BITS_PER_UNIT);
        }
    }

    // set all bits
    void set() {
        memset(arr, 1, units * sizeof(Unit));
    }

    // clear n bits
    void clear(size_t index, size_t bits = 1) {
        assert(index + bits <= size());

        for (size_t bit = index; bit < index + bits; ++bit) {
            arr[bit / BITS_PER_UNIT] &= ~(1 << (bit % BITS_PER_UNIT));
        }
    }

    // clear all bits
    void clear() {
        memset(arr, 0, units * sizeof(Unit));
    }

    // are n bits starting from index set to expected?
    bool test(size_t index, size_t bits = 1, bool expected = true) const {
        assert(index + bits < size());

        for (size_t bit = index; bit < index + bits; ++bit) {
            if (!!(arr[bit / BITS_PER_UNIT] & (1 << (bit % BITS_PER_UNIT))) != expected)
                return false;
        }

        return true;
    }

    // return index of first n free bits
    Optional<size_t> findFree(size_t bits) const {
        assert(bits <= size());

        for (size_t index = 0; index < size() - bits; ++index) {
            if (test(index, bits, false))
                return index;
        }

        return { };
    }

private:
    size_t units;
    Unit* arr;
};

#endif // BITSET_HPP
