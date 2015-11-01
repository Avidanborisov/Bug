#include "virtualallocator.hpp"
#include "physicalallocator.hpp"
#include "memorymap.hpp"

Bitset<> VirtualAllocator::pages;

void VirtualAllocator::init() {
    constexpr auto PAGES = 0x10000000 / Paging::PAGE_SIZE;

    pages.init(PAGES, Paging::PAGE_SIZE, PhysicalAllocator::reserve);
    for (const auto& hole : MemoryMap::getHoles())
        pages.set(hole.base / Paging::PAGE_SIZE, hole.length / Paging::PAGE_SIZE);
}

void VirtualAllocator::finalize() {
    pages.set(0, PhysicalAllocator::getKernelEnd() / Paging::PAGE_SIZE);
}

uint32_t VirtualAllocator::allocate(size_t blocks) {
    uint32_t page = pages.findFree(blocks);
    pages.set(page, blocks);
    return page * Paging::PAGE_SIZE;
}

void VirtualAllocator::free(uint32_t address, size_t blocks) {
    pages.clear(address / Paging::PAGE_SIZE, blocks);
}

