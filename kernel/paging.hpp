#ifndef PAGING_HPP
#define PAGING_HPP

#include <stdint.h>
#include <stddef.h>
#include "optional.hpp"
#include "math.hpp"
#include "task.h"

class Paging {
public:
    static constexpr uint32_t PAGE_SIZE = 4096;
    static constexpr uint32_t PAGE_MASK = 0xfffff000;

    static constexpr uint32_t PAGING_BIT = 0x80000000;

    constexpr static uint32_t alignDown(uint32_t address) {
        return Math::roundDown(address, PAGE_SIZE);
    }

    template<class T>
    constexpr static uint32_t alignDown(T* address) {
        return alignDown(reinterpret_cast<uint32_t>(address));
    }

    constexpr static uint32_t alignUp(uint32_t address) {
        return Math::roundUp(address, PAGE_SIZE);
    }

    template<class T>
    constexpr static uint32_t alignUp(T* address) {
        return alignUp(reinterpret_cast<uint32_t>(address));
    }

    constexpr static bool isAligned(uint32_t address) {
        return address % PAGE_SIZE == 0;
    }

    enum class Flags : uint32_t {
        NONE           = 0x00,
        PRESENT        = 0x01, // table/directory entry is present
        WRITE          = 0x02, // writes are allowed to the area referenced by the table/directory entry
        USER           = 0x04, // user-mode access is allowed to the area referenced by the table/directory entry
        WRITE_THROUGH  = 0x08,
        CACHE_DISABLED = 0x10,
        ACCESSED       = 0x20, // indicates whether software has accessed the area referenced by the table/directory entry
    };

    static void init();
    static uint32_t getKernelDirectory();

    static bool isMapped(uint32_t virtualAddress);
    static bool isMapped(uint32_t virtualAddress, size_t pages);

    static Optional<uint32_t> findFree(size_t pages, uint32_t startAddress, uint32_t endAddress);

    template<class T>
    static T findFree(size_t pages, uint32_t startAddress, uint32_t endAddress) {
        auto virtualAddress = findFree(pages, startAddress, endAddress);
        if (!virtualAddress)
            return nullptr;

        return reinterpret_cast<T>(*virtualAddress);
    }

    static bool map(uint32_t virtualAddress, uint32_t physicalAddress, Flags flags = Flags::NONE);
    static bool map(uint32_t virtualAddress, uint32_t physicalAddress, size_t pages, Flags flags = Flags::NONE);

    template<class T, class U>
    static bool map(T* virtualAddress, U* physicalAddress, Flags flags = Flags::NONE) {
        return map(reinterpret_cast<uint32_t>(virtualAddress), reinterpret_cast<uint32_t>(physicalAddress), flags);
    }

    template<class T, class U>
    static bool map(T* virtualAddress, U* physicalAddress, size_t pages, Flags flags = Flags::NONE) {
        return map(reinterpret_cast<uint32_t>(virtualAddress), reinterpret_cast<uint32_t>(physicalAddress), pages, flags);
    }

    static bool unmap(uint32_t virtualAddress);
    static bool unmap(uint32_t virtualAddress, size_t pages);

    template<class T>
    static bool unmap(T* virtualAddress) {
        return unmap(reinterpret_cast<uint32_t>(virtualAddress));
    }

    template<class T>
    static bool unmap(T* virtualAddress, size_t pages) {
        return unmap(reinterpret_cast<uint32_t>(virtualAddress), pages);
    }

    static void mapKernel(Task& task);

    static bool map(Task& task, uint32_t virtualAddress, uint32_t physicalAddress, Flags flags = Flags::NONE);
    static bool map(Task& task, uint32_t virtualAddress, uint32_t physicalAddress, size_t pages, Flags flags = Flags::NONE);

    template<class T, class U>
    static bool map(Task& task, T* virtualAddress, U* physicalAddress, Flags flags = Flags::NONE) {
        return map(task, reinterpret_cast<uint32_t>(virtualAddress), reinterpret_cast<uint32_t>(physicalAddress), flags);
    }

    template<class T, class U>
    static bool map(Task& task, T* virtualAddress, U* physicalAddress, size_t pages, Flags flags = Flags::NONE) {
        return map(task, reinterpret_cast<uint32_t>(virtualAddress), reinterpret_cast<uint32_t>(physicalAddress), pages, flags);
    }

    static bool unmap(Task& task, uint32_t virtualAddress);
    static bool unmap(Task& task, uint32_t virtualAddress, size_t pages);

    template<class T>
    static bool unmap(Task& task, T* virtualAddress) {
        return unmap(task, reinterpret_cast<uint32_t>(virtualAddress));
    }

    template<class T>
    static bool unmap(Task& task, T* virtualAddress, size_t pages) {
        return unmap(task, reinterpret_cast<uint32_t>(virtualAddress), pages);
    }

private:
    struct Indexes {
        size_t pde, pte;
    };

    static Indexes parseAddress(uint32_t virtualAddress);
    static bool isPresent(uint32_t virtualAddress);

    template<bool isPagingEnabled>
    static bool modify(uint32_t virtualAddress, uint32_t physicalAddress, Flags flags);

    static bool modify(Task& task, uint32_t virtualAddress, uint32_t physicalAddress, Flags flags);

    static void identityMap(uint32_t startAddress, uint32_t endAddress, Flags flags = Flags::NONE);

    static constexpr size_t ENTRIES = 1024;

    struct Table {
        class Entry {
        public:
            constexpr Entry() : value(0) { }
            Entry(uint32_t address, Flags flags = Flags::NONE);

            uint32_t getPhysical() const;

            template<Paging::Flags flags>
            constexpr bool is() const;

            template<bool isPagingEnabled>
            static Entry* get(uint32_t virtualAddress, bool create);

        private:
            uint32_t value;
        };

        static Entry* allocate();       // create a new page table (1024 page table entries)
        static void init(Entry* first); // initialize page table
    };

    static_assert(sizeof(Table::Entry) == 4, "Page Table Entry is 4 bytes long");
    static_assert(sizeof(Table::Entry) * ENTRIES == PAGE_SIZE, "Page Table takes whole page");

    struct Directory {
        class Entry {
        public:
            constexpr Entry() : value(0) { }
            Entry(Table::Entry* table, Flags flags = Flags::NONE);

            Table::Entry* getTablePhysical() const;

            template<Paging::Flags flags>
            constexpr bool is() const;

        private:
            uint32_t value;
        };

        static Entry* allocate();       // create a new page directory (1024 page directory entries)
        static void init(Entry* first); // initialize page table
    };

    static_assert(sizeof(Directory::Entry) == 4, "Page Directory Entry is 4 bytes long");
    static_assert(sizeof(Directory::Entry) * ENTRIES == PAGE_SIZE, "Page Directory takes whole page");

    static Directory::Entry* directory;
    static Directory::Entry* physicalKernelDirectory;
    static uint32_t firstFree;

    static constexpr auto VIRTUAL_DIRECTORY      = reinterpret_cast<Directory::Entry*>(0xfffff000);
    static constexpr auto VIRTUAL_TABLES         = reinterpret_cast<Table::Entry*>(0xffc00000);

    static constexpr auto VIRTUAL_TEMP_DIRECTORY = reinterpret_cast<Directory::Entry*>(0xffbff000);
    static constexpr auto VIRTUAL_TEMP_TABLE     = reinterpret_cast<Table::Entry*>(0xffbfe000);

public:
    static constexpr uint32_t VIRTUAL_USERSPACE_BEGIN = 0x40000000; // 1GiB
    static constexpr uint32_t VIRTUAL_USERSPACE_END   = reinterpret_cast<uint32_t>(VIRTUAL_TEMP_TABLE);
};

inline uint32_t& operator|=(uint32_t& lhs, Paging::Flags rhs) {
    lhs = static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
    return lhs;
}

inline constexpr uint32_t operator|(uint32_t lhs, Paging::Flags rhs) {
    return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

inline Paging::Flags& operator|=(Paging::Flags& lhs, Paging::Flags rhs) {
    lhs = static_cast<Paging::Flags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    return lhs;
}

inline constexpr Paging::Flags operator|(Paging::Flags lhs, Paging::Flags rhs) {
    return static_cast<Paging::Flags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline uint32_t& operator&=(uint32_t& lhs, Paging::Flags rhs) {
    lhs = static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
    return lhs;
}

inline constexpr uint32_t operator&(uint32_t lhs, Paging::Flags rhs) {
    return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}

inline Paging::Flags& operator&=(Paging::Flags& lhs, Paging::Flags rhs) {
    lhs = static_cast<Paging::Flags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
    return lhs;
}

inline constexpr Paging::Flags operator&(Paging::Flags lhs, Paging::Flags rhs) {
    return static_cast<Paging::Flags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

#endif // PAGING_HPP
