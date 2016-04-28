#ifndef VIRTUALALLOCATOR_HPP
#define VIRTUALALLOCATOR_HPP

#include <stdint.h>
#include <stddef.h>
#include "scheduler.hpp"
#include "paging.hpp"

class VirtualAllocator {
public:
    static uint32_t get(size_t pages, Paging::Flags flags = Paging::Flags::NONE);
    static uint32_t linked(size_t pages, Paging::Flags flags = Paging::Flags::NONE);
    static uint32_t linked(Task& task, size_t pages, Paging::Flags flags = Paging::Flags::NONE);
    static bool link(uint32_t virt, size_t pages, Paging::Flags flags = Paging::Flags::NONE);
    static Vector<Task::Segment> link(Task& task, uint32_t virt, size_t pages, Paging::Flags flags = Paging::Flags::NONE);
    static bool free(uint32_t address, size_t pages);

    template<class T>
    static T get(size_t pages, Paging::Flags flags = Paging::Flags::NONE) {
        return reinterpret_cast<T>(get(pages, flags));
    }

    template<class T>
    static T linked(size_t pages, Paging::Flags flags = Paging::Flags::NONE) {
        return reinterpret_cast<T>(linked(pages, flags));
    }

    template<class T>
    static T linked(Task& task, size_t pages, Paging::Flags flags = Paging::Flags::NONE) {
        return reinterpret_cast<T>(linked(task, pages, flags));
    }

    template<class T>
    static bool free(T* address, size_t pages) {
        return free(reinterpret_cast<uint32_t>(address), pages);
    }

private:
    static uint32_t linked(size_t pages, Paging::Flags flags, Task* task);
    static bool link(uint32_t virt, size_t pages, Task* task, Paging::Flags flags, Vector<Task::Segment>* out = nullptr);
};

#endif // VIRTUALALLOCATOR_HPP
