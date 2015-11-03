#ifndef PAGING_HPP
#define PAGING_HPP

#include <stdint.h>
#include <stddef.h>
#include "math.hpp"

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
        USER           = 0x04, // user-mode access is allowed to the area reference by the table/directory entry
        WRITE_THROUGH  = 0x08,
        CACHE_DISABLED = 0x10,
        ACCESSED       = 0x20, // indicates whether software has accessed the area reference by the table/directory entry
    };

    static void init();

    static bool map(uint32_t virtualAddress, uint32_t physicalAddress, Flags flags = Flags::NONE);
    static bool map(uint32_t virtualAddress, uint32_t physicalAddress, size_t pages, Flags flags = Flags::NONE);

    static bool unmap(uint32_t virtualAddress);
    static bool unmap(uint32_t virtualAddress, size_t pages);

private:
    struct Indexes {
        uint32_t pde, pte;
    };

    static Indexes parseAddress(uint32_t virtualAddress);
    static bool isPresent(uint32_t virtualAddress);

    template<bool isPagingEnabled>
    static bool modify(uint32_t virtualAddress, uint32_t physicalAddress, Flags flags);

    static void identityMap(uint32_t startAddress, uint32_t endAddress, Flags flags = Flags::NONE);

    static constexpr size_t ENTRIES = 1024;

    struct Table {
        class Entry {
        public:
            constexpr Entry() : value(0) { }
            Entry(uint32_t address, Flags flags = Flags::NONE);

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

    static constexpr auto VIRTUAL_DIRECTORY = reinterpret_cast<Directory::Entry*>(0xfffff000);
    static constexpr auto VIRTUAL_TABLES    = reinterpret_cast<Table::Entry*>(0xffc00000);
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
