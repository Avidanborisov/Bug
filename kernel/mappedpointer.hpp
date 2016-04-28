#ifndef MAPPEDPOINTER_HPP
#define MAPPEDPOINTER_HPP

#include "virtualallocator.hpp"

template<class T>
class MappedPointer {
public:
    template<class U>
    MappedPointer(U physicalAddress, T* virtualAddress, size_t pages = 1, Paging::Flags flags = Paging::Flags::NONE) :
        pages(pages) {
        if (Paging::map(reinterpret_cast<T*>(virtualAddress), reinterpret_cast<T*>(physicalAddress), pages, flags)) {
            virt = reinterpret_cast<T*>(virtualAddress);
        } else {
            virt = nullptr;
        }
    }

    template<class U>
    MappedPointer(U phys, size_t pages = 1, Paging::Flags flags = Paging::Flags::NONE) :
        pages(pages) {
        virt = VirtualAllocator::get<T*>(pages);
        if (!virt) {
            virt = nullptr;
            return;
        }

        if (!Paging::map(reinterpret_cast<T*>(virt), reinterpret_cast<T*>(phys), pages, flags)) {
            virt = nullptr;
        }
    }

    ~MappedPointer() {
        if (virt) {
            Paging::unmap(virt, pages);
        }
    }

    T* get() const {
        return virt;
    }

    T& operator*() const {
        return *virt;
    }

    T* operator->() const {
        return virt;
    }

    T& operator[](size_t i) const {
        return virt[i];
    }

    operator bool() const {
        return virt != nullptr;
    }

private:
    T* virt;
    size_t pages;
};

#endif // MAPPEDPOINTER_HPP
