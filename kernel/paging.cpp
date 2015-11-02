#include "paging.hpp"
#include "physicalallocator.hpp"
#include "virtualallocator.hpp"
#include "x86.hpp"
#include "console.hpp"
#include "isr.hpp"
#include "memorymap.hpp"

Paging::Directory::Entry* Paging::directory;

void Paging::init() {
    directory = Directory::allocate(PhysicalAllocator::reserve);
    Directory::init(directory);

    // identity map kernel code and data
    identityMap(0, PhysicalAllocator::getKernelEnd, Flags::USER);

    // identity map framebuffer video memory
    identityMap(alignDown(Framebuffer::PHYSICAL_MEMORY_START), [&] {
        return alignUp(Framebuffer::PHYSICAL_MEMORY_END);
    }, Flags::USER);

    // recursive page directory mapping - map last pde to point to the page directory.
    // since pde's and pte's are in compatiable format, this allows accessing physical
    // table addresses after paging is enabled.
    directory[ENTRIES - 1] = Directory::Entry(reinterpret_cast<Table::Entry*>(directory));

    // The top 4MiB are reserved for the page tables pointed by the last directory entry
    VirtualAllocator::exclude(reinterpret_cast<uint32_t>(VIRTUAL_TABLES), ENTRIES * PAGE_SIZE);

    x86::regs::cr3 = reinterpret_cast<uint32_t>(directory);
    x86::regs::cr0 |= PAGING_BIT;
}

void Paging::map(uint32_t virtualAddress, uint32_t physicalAddress, Paging::Flags flags) {
    map<true>(virtualAddress, physicalAddress, flags);
}

Paging::Indexes Paging::parseAddress(uint32_t address) {
    return { address / PAGE_SIZE / ENTRIES, address / PAGE_SIZE % ENTRIES };
}

template<bool isPagingEnabled>
void Paging::map(uint32_t virtualAddress, uint32_t physicalAddress, Paging::Flags flags) {
    auto indexes = parseAddress(virtualAddress);
    Directory::Entry& pde = directory[indexes.pde];

    auto getTable = [&] {
        return isPagingEnabled ? &VIRTUAL_TABLES[ENTRIES * indexes.pde] : pde.getTablePhysical();
    };

    if (!pde.is<Flags::PRESENT>()) {
        auto table = Table::allocate(isPagingEnabled ? PhysicalAllocator::allocate : PhysicalAllocator::reserve);
        pde = Directory::Entry(table, Flags::WRITE | Flags::USER);
        Table::init(getTable());
    }

    Table::Entry& pte = getTable()[indexes.pte];
    pte = Table::Entry(physicalAddress, flags);
}

template<class Func>
void Paging::identityMap(uint32_t startAddress, Func getEndAddress, Paging::Flags flags) {
    for (size_t addr = startAddress; addr < getEndAddress(); addr += PAGE_SIZE)
        map<false>(addr, addr, flags);

    VirtualAllocator::exclude(startAddress, getEndAddress() - startAddress);
}

Paging::Table::Entry::Entry(uint32_t address, Flags flags) : value(0) {
    value |= flags | Flags::PRESENT; // set flags
    value |= address & PAGE_MASK; // set address
}

template<Paging::Flags flags>
constexpr bool Paging::Table::Entry::is() const {
    return value & flags;
}

template<class PageAllocator>
Paging::Table::Entry* Paging::Table::allocate(PageAllocator alloc) {
    return reinterpret_cast<Entry*>(alloc(1)); // allocate 1 page
}

void Paging::Table::init(Paging::Table::Entry* first) {
    auto table = first;

    // initialize all entries
    for (size_t i = 0; i < ENTRIES; ++i)
        table[i] = Entry();
}

Paging::Directory::Entry::Entry(Paging::Table::Entry* table, Paging::Flags flags) : value (0) {
    value |= flags | Flags::PRESENT; // set flags
    value |= reinterpret_cast<uint32_t>(table) & PAGE_MASK; // set address
}

Paging::Table::Entry* Paging::Directory::Entry::getTablePhysical() const {
    return reinterpret_cast<Table::Entry*>(value & PAGE_MASK);
}

template<Paging::Flags flags>
constexpr bool Paging::Directory::Entry::is() const {
    return value & flags;
}

template<class PageAllocator>
Paging::Directory::Entry* Paging::Directory::allocate(PageAllocator alloc) {
    return reinterpret_cast<Entry*>(alloc(1)); // allocate 1 page
}

void Paging::Directory::init(Paging::Directory::Entry* first) {
    auto directory = first;

    // initialize all entries
    for (size_t i = 0; i < ENTRIES; ++i)
        directory[i] = Entry();
}
