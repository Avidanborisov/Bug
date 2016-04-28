#include "paging.hpp"
#include "physicalallocator.hpp"
#include "virtualallocator.hpp"
#include "x86.hpp"
#include "framebuffer.hpp"
#include "mappedpointer.hpp"
#include "kernel.hpp"

Paging::Directory::Entry* Paging::directory;
uint32_t Paging::firstFree;

void Paging::init() {
    directory = Directory::allocate();
    Directory::init(directory);

    // identity map kernel code and data
    identityMap(0, PhysicalAllocator::getKernelEnd(), Flags::WRITE);
    firstFree = PhysicalAllocator::getKernelEnd();

    // identity map framebuffer video memory
    identityMap(alignDown(Framebuffer::PHYSICAL_MEMORY_START), alignUp(Framebuffer::PHYSICAL_MEMORY_END), Flags::USER | Flags::WRITE);

    // recursive page directory mapping - map last pde to point to the page directory.
    // since pde's and pte's are in compatiable format, this allows accessing physical
    // table addresses after paging is enabled.
    directory[ENTRIES - 1] = Directory::Entry(reinterpret_cast<Table::Entry*>(directory));

    x86::regs::cr3 = reinterpret_cast<uint32_t>(directory);
    x86::regs::cr0 |= PAGING_BIT;

    directory = VIRTUAL_DIRECTORY;
}

bool Paging::isMapped(uint32_t virtualAddress) {
    auto pte = Table::Entry::get<true>(virtualAddress, false);
    if (!pte)
        return false;

    return pte->is<Flags::PRESENT>();
}

bool Paging::isMapped(uint32_t virtualAddress, size_t pages) {
    for (size_t page = 0; page < pages; ++page) {
        if (isMapped(virtualAddress + page * PAGE_SIZE))
            return true;
    }

    return false;
}

Optional<uint32_t> Paging::findFree(size_t pages, uint32_t startAddress, uint32_t endAddress) {
    if (startAddress == 0)
        startAddress = firstFree;

    endAddress -= pages * PAGE_SIZE;

    for (uint32_t address = startAddress; address <= endAddress; address += PAGE_SIZE)
        if (!isMapped(address, pages))
            return address;

    return { };
}

bool Paging::map(uint32_t virtualAddress, uint32_t physicalAddress, Paging::Flags flags) {
    return modify<true>(virtualAddress, physicalAddress, flags | Flags::PRESENT);
}

bool Paging::map(uint32_t virtualAddress, uint32_t physicalAddress, size_t pages, Paging::Flags flags) {
    if (isMapped(virtualAddress, pages))
        return false;

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
    for (size_t page = 0; page < pages; ++page) {
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

    Table::Entry& pte = getTable()[indexes.pte];
    return &pte;
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

        if (virtualAddress < firstFree)
            firstFree = virtualAddress;
    } else {
        if (operation == UNMAP)
            return false;

        *pte = Table::Entry(physicalAddress, flags);

        if (virtualAddress == firstFree + PAGE_SIZE)
            firstFree += PAGE_SIZE;
    }

    x86::invlpg(virtualAddress); // flush virtual address in TLB
    return true;
}

void Paging::identityMap(uint32_t startAddress, uint32_t endAddress, Paging::Flags flags) {
    for (size_t addr = startAddress; addr < endAddress; addr += PAGE_SIZE)
        modify<false>(addr, addr, flags | Flags::PRESENT);
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
    return PhysicalAllocator::allocate<Entry*>(1); // allocate 1 page = entire page table
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
    return PhysicalAllocator::allocate<Entry*>(1); // allocate 1 page = entire directory
}

void Paging::Directory::init(Paging::Directory::Entry* first) {
    auto directory = first;

    // initialize all entries
    for (size_t i = 0; i < ENTRIES; ++i)
        directory[i] = Entry();
}

void Paging::mapKernel(Task& task) {
    task.cr3 = PhysicalAllocator::allocate(1);
    MappedPointer<Directory::Entry> newDirectory(task.cr3, VIRTUAL_TEMP_DIRECTORY);

    Directory::init(newDirectory.get());

    for (size_t addr = 0; addr < VIRTUAL_USERSPACE_BEGIN; addr += PAGE_SIZE * ENTRIES) {
        auto pde = parseAddress(addr).pde;
        newDirectory[pde] = directory[pde];
    }
}

bool Paging::modify(Task& task, uint32_t virtualAddress, uint32_t physicalAddress, Paging::Flags flags) {
    const enum {
        MAP, UNMAP
    } operation = (flags == Flags::NONE) ? UNMAP : MAP;

    if (!isAligned(virtualAddress))
        return false;

    MappedPointer<Directory::Entry> directory(task.cr3, VIRTUAL_TEMP_DIRECTORY);

    auto indexes = parseAddress(virtualAddress);
    Directory::Entry& pde = directory[indexes.pde];

    if (!pde.is<Flags::PRESENT>() && operation == UNMAP) {
        Kernel::panic("failed mapping 1");
        return false;
    }

    auto tablePhysical = pde.is<Flags::PRESENT>() ? pde.getTablePhysical() : Table::allocate();
    MappedPointer<Table::Entry> table(tablePhysical, VIRTUAL_TEMP_TABLE);

    if (!pde.is<Flags::PRESENT>() && operation == MAP) {
        Table::init(table.get());
        pde = Directory::Entry(tablePhysical, Flags::WRITE | Flags::USER);
        task.segments.push({ reinterpret_cast<uint32_t>(tablePhysical), 0, 1 });
    }

    Table::Entry& pte = table[indexes.pte];
    if (pte.is<Flags::PRESENT>()) {
        if (operation == MAP) {
            Kernel::panic("failed mapping 2");
            return false;
        }

        pte = Table::Entry(); // empty entry (not present)
    } else {
        if (operation == UNMAP) {
            Kernel::panic("failed mapping 3");
            return false;
        }

        pte = Table::Entry(physicalAddress, flags);
    }

    return true;
}


bool Paging::map(Task& task, uint32_t virtualAddress, uint32_t physicalAddress, Paging::Flags flags) {
    return modify(task, virtualAddress, physicalAddress, flags | Flags::PRESENT);
}

bool Paging::map(Task& task, uint32_t virtualAddress, uint32_t physicalAddress, size_t pages, Paging::Flags flags) {
    for (size_t page = 0; page < pages; ++page) {
        if (!map(task, virtualAddress + page * PAGE_SIZE, physicalAddress + page * PAGE_SIZE, flags)) {
            while (page-- > 0)
                Paging::unmap(task, virtualAddress + page * Paging::PAGE_SIZE);

            return false;
        }
    }

    return true;
}

bool Paging::unmap(Task& task, uint32_t virtualAddress) {
    return modify(task, virtualAddress, 0, Flags::NONE);
}

bool Paging::unmap(Task& task, uint32_t virtualAddress, size_t pages) {
    for (size_t page = 0; page < pages; ++page) {
        if (!unmap(task, virtualAddress + page * PAGE_SIZE))
            return false;
    }

    return true;
}
