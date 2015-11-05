#include "virtualallocator.hpp"
#include "physicalallocator.hpp"
#include "memorymap.hpp"

Optional<uint32_t> VirtualAllocator::allocate(size_t pages) {
    auto virt = Paging::findFree(pages);
    if (!virt)
        return { };

    for (size_t page = 0; page < pages; ++page) {
        auto phys = PhysicalAllocator::allocate(1);

        if (!Paging::map(*virt + page * Paging::PAGE_SIZE, phys)) {
            while (page-- > 0)
                Paging::unmap(*virt + page * Paging::PAGE_SIZE);

            return { };
        }

    }

    return virt;
}

bool VirtualAllocator::free(uint32_t address, size_t blocks) {
    return Paging::unmap(address, blocks);
}

