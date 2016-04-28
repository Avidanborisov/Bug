#include "virtualallocator.hpp"
#include "physicalallocator.hpp"
#include "memorymap.hpp"

uint32_t VirtualAllocator::get(size_t pages, Paging::Flags flags) {
    Optional<uint32_t> virt;
    if (bool(flags & Paging::Flags::USER_SPACE)) {
        virt = Paging::findFree(pages, Paging::VIRTUAL_USERSPACE_BEGIN, Paging::VIRTUAL_USERSPACE_END);
    } else {
        virt = Paging::findFree(pages, 0, Paging::VIRTUAL_USERSPACE_BEGIN);
    }

    return virt ? *virt : 0;
}

uint32_t VirtualAllocator::linked(size_t pages, Paging::Flags flags) {
    return linked(pages, flags, nullptr);
}

uint32_t VirtualAllocator::linked(Task& task, size_t pages, Paging::Flags flags) {
    return linked(pages, flags, &task);
}

uint32_t VirtualAllocator::linked(size_t pages, Paging::Flags flags, Task* task) {
    auto virt = get(pages, flags);
    if (!virt)
        return 0;

    if (link(virt, pages, task, flags)) {
        return virt;
    } else {
        return 0;
    }
}

bool VirtualAllocator::link(uint32_t virt, size_t pages, Paging::Flags flags) {
    return link(virt, pages, nullptr, flags);
}

Vector<Task::Segment> VirtualAllocator::link(Task& task, uint32_t virt, size_t pages, Paging::Flags flags) {
    Vector<Task::Segment> added;
    link(virt, pages, &task, flags, &added);
    return added;
}

bool VirtualAllocator::link(uint32_t virt, size_t pages, Task* task, Paging::Flags flags, Vector<Task::Segment>* added) {
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
    return VirtualAllocator::linked<void*>(pages);
}
