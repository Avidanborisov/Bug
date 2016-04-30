#include "physicalallocator.hpp"
#include "console.hpp"
#include "paging.hpp"
#include "memorymap.hpp"
#include "kernel.hpp"
#include "sysfs.hpp"

extern uint32_t kernelEnd;
uint32_t PhysicalAllocator::kernelEnd = Paging::alignUp(&::kernelEnd);
bool PhysicalAllocator::initialized = false;
size_t PhysicalAllocator::usedPages = 0;

Bitset<> PhysicalAllocator::memory;

uint32_t PhysicalAllocator::reserve(size_t pages) {
    assert(!initialized);

    // if reserved memory is in hole
    if (auto hole = MemoryMap::isInHole(kernelEnd, pages * Paging::PAGE_SIZE))
        kernelEnd = hole->base + hole->length; // jump over to free region

    auto address = kernelEnd;
    kernelEnd += pages * Paging::PAGE_SIZE;
    return address;
}

uint32_t PhysicalAllocator::getKernelEnd() {
    return kernelEnd;
}

void PhysicalAllocator::init() {
    const auto PAGES = MemoryMap::getEnd() / Paging::PAGE_SIZE;

    memory.init(PAGES, Paging::PAGE_SIZE, reserve);
    for (const auto& hole : MemoryMap::getHoles())
        memory.set(hole.base / Paging::PAGE_SIZE, hole.length / Paging::PAGE_SIZE);

    memory.set(0, kernelEnd / Paging::PAGE_SIZE);
    initialized = true;
    usedPages = kernelEnd / Paging::PAGE_SIZE;
}

uint32_t PhysicalAllocator::allocate(size_t pages) {
    auto page = memory.findFree(pages);
    if (!page) {
        if (pages == 1)
            Kernel::panic("No free physical memory!");

        return 0;
    }

    usedPages += pages;
    memory.set(*page, pages);
    return *page * Paging::PAGE_SIZE;
}

void PhysicalAllocator::free(uint32_t address, size_t pages) {
    memory.clear(address / Paging::PAGE_SIZE, pages);
    usedPages -= pages;
}

size_t PhysicalAllocator::used() {
    return usedPages * Paging::PAGE_SIZE;
}

size_t PhysicalAllocator::available() {
    return memory.size() * Paging::PAGE_SIZE;
}
