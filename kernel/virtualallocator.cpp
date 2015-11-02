#include "virtualallocator.hpp"
#include "physicalallocator.hpp"
#include "memorymap.hpp"

Bitset<> VirtualAllocator::pages;

void VirtualAllocator::init() {
    constexpr auto PAGES = MemoryMap::MAX_MEMORY / Paging::PAGE_SIZE;
    pages.init(PAGES, Paging::PAGE_SIZE, PhysicalAllocator::reserve);
}

void VirtualAllocator::exclude(uint32_t base, size_t length) {
    assert(Paging::isAligned(base) && Paging::isAligned(length));
    pages.set(base / Paging::PAGE_SIZE, length / Paging::PAGE_SIZE);
}

uint32_t VirtualAllocator::allocate(size_t blocks) {
    uint32_t page = pages.findFree(blocks);
    pages.set(page, blocks);
    return page * Paging::PAGE_SIZE;
}

void VirtualAllocator::free(uint32_t address, size_t blocks) {
    assert(Paging::isAligned(address));
    pages.clear(address / Paging::PAGE_SIZE, blocks);
}

