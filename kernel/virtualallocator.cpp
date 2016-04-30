#include "virtualallocator.hpp"
#include "physicalallocator.hpp"
#include "kernel.hpp"

uint32_t VirtualAllocator::getFreeKernelPages(size_t pages) {
    Optional<uint32_t> addr = Paging::findFree(pages, 0, Paging::VIRTUAL_USERSPACE_BEGIN);
    return addr ? *addr : 0;
}

uint32_t VirtualAllocator::getMappedKernelPages(size_t pages, Paging::Flags flags) {
    return getMappedKernelPages(pages, flags, nullptr);
}

uint32_t VirtualAllocator::getMappedKernelPages(Task& task, size_t pages, Paging::Flags flags) {
    return getMappedKernelPages(pages, flags, &task);
}

uint32_t VirtualAllocator::getMappedKernelPages(size_t pages, Paging::Flags flags, Task* task) {
    auto virt = getFreeKernelPages(pages);
    if (!virt)
        return 0;

    if (createPhysicalMapping(virt, pages, task, flags)) {
        return virt;
    } else {
        return 0;
    }
}

bool VirtualAllocator::createPhysicalMapping(uint32_t virt, size_t pages, Paging::Flags flags) {
    return createPhysicalMapping(virt, pages, nullptr, flags);
}

Vector<Task::Segment> VirtualAllocator::createPhysicalMapping(Task& task, uint32_t virt, size_t pages, Paging::Flags flags) {
    Vector<Task::Segment> added;
    createPhysicalMapping(virt, pages, &task, flags, &added);
    return added;
}

bool VirtualAllocator::createPhysicalMapping(uint32_t virt, size_t pages, Task* task, Paging::Flags flags, Vector<Task::Segment>* added) {
    auto map = [&](uint32_t virt, uint32_t phys, size_t pages) {
        if (task) {
            if (added) {
                added->push({ phys, virt, pages });
            }

            task->segments.push({ phys, virt, pages });
            return Paging::map(*task, virt, phys, pages, flags | Paging::Flags::USER);
        } else {
            return Paging::map(virt, phys, pages);
        }
    };

    auto unmap = [&](uint32_t virt) {
        if (task) {
            return Paging::unmap(*task, virt);
        } else {
            return Paging::unmap(virt);
        }
    };

    auto phys = PhysicalAllocator::allocate(pages);
    if (phys) {
        map(virt, phys, pages);
    } else { // try getting not consecutive physical memory
        for (size_t page = 0; page < pages; ++page) {
            auto phys = PhysicalAllocator::allocate(1);

            if (!map(virt + page * Paging::PAGE_SIZE, phys, 1)) {
                while (page-- > 0)
                    unmap(virt + page * Paging::PAGE_SIZE);

                return false;
            }
        }
    }

    return true;
}

void* allocator(size_t pages) {
    auto virt = VirtualAllocator::getMappedKernelPages<void*>(pages);
    if (!virt)
        Kernel::panic("No free virtual pages in kernel space");

    return virt;
}
