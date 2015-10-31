#include "physicalallocator.hpp"
#include "console.hpp"
#include "paging.hpp"
#include "memorymap.hpp"

extern uint32_t kernelEnd;
uint32_t PhysicalAllocator::kernelEnd = Paging::alignUp(&kernelEnd);
bool PhysicalAllocator::initialized = false;

Bitset<> PhysicalAllocator::AddressBlockMap::memory;
uint32_t PhysicalAllocator::AddressBlockMap::end;

uint32_t PhysicalAllocator::reserve(size_t blocks) {
    assert(!initialized);

    auto address = kernelEnd;
    kernelEnd += blocks * BLOCK_SIZE;
    return address;
}

uint32_t PhysicalAllocator::getKernelEnd() {
    return kernelEnd;
}

void PhysicalAllocator::init() {
    AddressBlockMap::init(MemoryMap::getEnd());

    for (const auto& hole : MemoryMap::getHoles())
        AddressBlockMap::mark(hole.base, hole.length / BLOCK_SIZE);

    AddressBlockMap::mark(0, kernelEnd / BLOCK_SIZE);
    initialized = true;
}

uint32_t PhysicalAllocator::allocate(size_t blocks) {
    uint32_t address = AddressBlockMap::findFreeBlocks(blocks);
    AddressBlockMap::mark(address, blocks);
    return address;
}

void PhysicalAllocator::free(uint32_t address, size_t blocks) {
    AddressBlockMap::free(address, blocks);
}

void PhysicalAllocator::AddressBlockMap::init(uint32_t endAddress) {
    end = endAddress / BLOCK_SIZE;
    memory.init(end, BLOCK_SIZE, PhysicalAllocator::reserve);
}

uint32_t PhysicalAllocator::AddressBlockMap::findFreeBlocks(size_t blocks) {
    return memory.findFree(blocks) * BLOCK_SIZE;
}

void PhysicalAllocator::AddressBlockMap::mark(uint32_t address, size_t blocks) {
    assert(address / BLOCK_SIZE + blocks <= end);
    memory.set(address / BLOCK_SIZE, blocks);
}

void PhysicalAllocator::AddressBlockMap::free(uint32_t address, size_t blocks) {
    assert(address / BLOCK_SIZE + blocks <= end);
    memory.clear(address / BLOCK_SIZE, blocks);
}
