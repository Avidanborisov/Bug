#ifndef VIRTUALALLOCATOR_HPP
#define VIRTUALALLOCATOR_HPP

#include <stdint.h>
#include <stddef.h>
#include "scheduler.hpp"
#include "paging.hpp"

class VirtualAllocator {
public:
    static uint32_t getFreeKernelPages(size_t pages);

    static uint32_t getMappedKernelPages(size_t pages, Paging::Flags flags = Paging::Flags::NONE);
    static uint32_t getMappedKernelPages(Task& task, size_t pages, Paging::Flags flags = Paging::Flags::NONE);

    static bool                  createPhysicalMapping(uint32_t virt, size_t pages, Paging::Flags flags = Paging::Flags::NONE);
    static Vector<Task::Segment> createPhysicalMapping(Task& task, uint32_t virt, size_t pages, Paging::Flags flags = Paging::Flags::NONE);

    template<class T>
    static T getFreeKernelPages(size_t pages) {
        return reinterpret_cast<T>(getFreeKernelPages(pages));
    }

    template<class T>
    static T getMappedKernelPages(size_t pages, Paging::Flags flags = Paging::Flags::NONE) {
        return reinterpret_cast<T>(getMappedKernelPages(pages, flags));
    }

    template<class T>
    static T getMappedKernelPages(Task& task, size_t pages, Paging::Flags flags = Paging::Flags::NONE) {
        return reinterpret_cast<T>(getMappedKernelPages(task, pages, flags));
    }

private:
    static uint32_t getMappedKernelPages(size_t pages, Paging::Flags flags, Task* task);
    static bool createPhysicalMapping(uint32_t virt, size_t pages, Task* task, Paging::Flags flags, Vector<Task::Segment>* out = nullptr);
};

#endif // VIRTUALALLOCATOR_HPP
