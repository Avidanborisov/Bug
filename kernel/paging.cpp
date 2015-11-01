#include "paging.hpp"
#include "physicalallocator.hpp"
#include "x86.hpp"
#include "console.hpp"
#include "isr.hpp"
#include "memorymap.hpp"

Paging::Directory::Entry* Paging::directory;

Paging::Table::Entry::Entry(uint32_t address, Flags flags) : value(0) {
    value |= flags | Flags::PRESENT;
    setAddress(address);
}

uint32_t Paging::Table::Entry::getAddress() const {
    return value & PAGE_MASK;
}

void Paging::Table::Entry::setAddress(uint32_t address) {
    value |= address & PAGE_MASK;
}

Paging::Table::Entry* Paging::Table::create() {
    auto table = reinterpret_cast<Entry*>(PhysicalAllocator::reserve(1)); // allocate 1 page

    // initialize all entries
    for (size_t i = 0; i < ENTRIES; ++i)
        table[i] = Entry();

    return table;
}

Paging::Directory::Entry::Entry(Paging::Table::Entry* table, Paging::Flags flags) : value (0) {
    value |= flags | Flags::PRESENT;
    setTable(table);
}

Paging::Table::Entry* Paging::Directory::Entry::getTable() const {
    return reinterpret_cast<Table::Entry*>(value & PAGE_MASK);
}

void Paging::Directory::Entry::setTable(Paging::Table::Entry* table) {
    value |= reinterpret_cast<uint32_t>(table) & PAGE_MASK;
}

Paging::Directory::Entry* Paging::Directory::create() {
    auto directory = reinterpret_cast<Entry*>(PhysicalAllocator::reserve(1)); // allocate 1 page

    // initialize all entries
    for (size_t i = 0; i < ENTRIES; ++i)
        directory[i] = Entry();

    return directory;
}

void Paging::init() {
    directory = Directory::create();

    // identity map kernel code and data
    identityMap(0, PhysicalAllocator::getKernelEnd, Flags::USER);

    // identity map all memory holes (to support framebuffer writes, etc..)
    for (const auto& hole : MemoryMap::getHoles())
        identityMap(hole.base, [&] { return hole.base + hole.length; });

    x86::regs::cr3 = reinterpret_cast<uint32_t>(directory);
    x86::regs::cr0 |= 0x80000000;
}

template<class Func>
void Paging::identityMap(uint32_t startAddress, Func getEndAddress, Paging::Flags flags) {
    for (size_t addr = startAddress; addr < getEndAddress(); addr += PAGE_SIZE) {
        Directory::Entry& pde = directory[addr / PAGE_SIZE / ENTRIES];

        if (pde.getTable() == nullptr)
            pde = Directory::Entry(Table::create(), Flags::WRITE | Flags::USER);

        auto table = pde.getTable();
        Table::Entry& pte = table[(addr / PAGE_SIZE) % ENTRIES];

        pte = Table::Entry(addr, flags);
    }
}
