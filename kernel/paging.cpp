#include "paging.hpp"
#include "physicalallocator.hpp"
#include "virtualallocator.hpp"
#include "x86.hpp"
#include "framebuffer.hpp"

Paging::Directory::Entry* Paging::directory;

void Paging::init() {
    directory = Directory::allocate();
    Directory::init(directory);

    // identity map kernel code and data
    identityMap(0, PhysicalAllocator::getKernelEnd(), Flags::USER);

    // identity map framebuffer video memory
    identityMap(alignDown(Framebuffer::PHYSICAL_MEMORY_START), alignUp(Framebuffer::PHYSICAL_MEMORY_END), Flags::USER);

    // recursive page directory mapping - map last pde to point to the page directory.
    // since pde's and pte's are in compatiable format, this allows accessing physical
    // table addresses after paging is enabled.
    directory[ENTRIES - 1] = Directory::Entry(reinterpret_cast<Table::Entry*>(directory));

    // The top 4MiB are reserved for the page tables pointed by the last directory entry
    VirtualAllocator::exclude(reinterpret_cast<uint32_t>(VIRTUAL_TABLES), ENTRIES * PAGE_SIZE);

    x86::regs::cr3 = reinterpret_cast<uint32_t>(directory);
    x86::regs::cr0 |= PAGING_BIT;

    directory = VIRTUAL_DIRECTORY;
}

bool Paging::map(uint32_t virtualAddress, uint32_t physicalAddress, Paging::Flags flags) {
    return modify<true>(virtualAddress, physicalAddress, flags | Flags::PRESENT);
}

bool Paging::map(uint32_t virtualAddress, uint32_t physicalAddress, size_t pages, Paging::Flags flags) {
    // first make sure all pages are currently unmapped to avoid mapping only a subset of them
    for (size_t page = 0; page < pages; ++page) {
        if (isPresent(virtualAddress + page * PAGE_SIZE))
            return false;
    }

    // actually map all pages
    for (size_t page = 0; page < pages; ++page) {
        if (!map(virtualAddress + page * PAGE_SIZE, physicalAddress + page * PAGE_SIZE, flags))
            return false;
    }

    return true;
}

bool Paging::unmap(uint32_t virtualAddress) {
    return modify<true>(virtualAddress, 0, Flags::NONE);
}

bool Paging::unmap(uint32_t virtualAddress, size_t pages) {
    for (size_t page; page < pages; ++page) {
        if (!unmap(virtualAddress + page * PAGE_SIZE))
            return false;
    }

    return true;
}

Paging::Indexes Paging::parseAddress(uint32_t virtualAddress) {
    return { virtualAddress / PAGE_SIZE / ENTRIES, virtualAddress / PAGE_SIZE % ENTRIES };
}

template<bool isPagingEnabled>
Paging::Table::Entry* Paging::Table::Entry::get(uint32_t virtualAddress, bool create) {
    if (!isAligned(virtualAddress))
        return nullptr;

    auto indexes = parseAddress(virtualAddress);
    Directory::Entry& pde = directory[indexes.pde];

    auto getTable = [&] {
        return isPagingEnabled ? &VIRTUAL_TABLES[ENTRIES * indexes.pde] : pde.getTablePhysical();
    };

    if (!pde.is<Flags::PRESENT>()) {
        if (!create)
            return nullptr;

        pde = Directory::Entry(Table::allocate(), Flags::WRITE | Flags::USER);
        auto table = getTable();

        x86::invlpg(table); // flush table address in TLB
        Table::init(table);
    }

    Entry& pte = getTable()[indexes.pte];
    return &pte;
}

bool Paging::isPresent(uint32_t virtualAddress) {
    auto pte = Table::Entry::get<true>(virtualAddress, false);
    if (!pte)
        return false;

    return pte->is<Flags::PRESENT>();
}

template<bool isPagingEnabled>
bool Paging::modify(uint32_t virtualAddress, uint32_t physicalAddress, Paging::Flags flags) {
    const enum {
        MAP, UNMAP
    } operation = (flags == Flags::NONE) ? UNMAP : MAP;

    Table::Entry* pte = Table::Entry::get<isPagingEnabled>(virtualAddress, operation == MAP);
    if (!pte)
        return false;

    if (pte->is<Flags::PRESENT>()) {
        if (operation == MAP)
            return false;

        *pte = Table::Entry(); // empty entry (not present)
    } else {
        if (operation == UNMAP)
            return false;

        *pte = Table::Entry(physicalAddress, flags);
    }

    x86::invlpg(virtualAddress); // flush virtual address in TLB
    return true;
}

void Paging::identityMap(uint32_t startAddress, uint32_t endAddress, Paging::Flags flags) {
    for (size_t addr = startAddress; addr < endAddress; addr += PAGE_SIZE)
        modify<false>(addr, addr, flags | Flags::PRESENT);

    VirtualAllocator::exclude(startAddress, endAddress - startAddress);
}

Paging::Table::Entry::Entry(uint32_t address, Flags flags) : value(0) {
    value |= flags | Flags::PRESENT; // set flags
    value |= address & PAGE_MASK; // set address
}

template<Paging::Flags flags>
constexpr bool Paging::Table::Entry::is() const {
    return value & flags;
}

Paging::Table::Entry* Paging::Table::allocate() {
    return reinterpret_cast<Entry*>(PhysicalAllocator::allocate(1)); // allocate 1 page = entire page table
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

Paging::Directory::Entry* Paging::Directory::allocate() {
    return reinterpret_cast<Entry*>(PhysicalAllocator::allocate(1)); // allocate 1 page = entire directory
}

void Paging::Directory::init(Paging::Directory::Entry* first) {
    auto directory = first;

    // initialize all entries
    for (size_t i = 0; i < ENTRIES; ++i)
        directory[i] = Entry();
}
