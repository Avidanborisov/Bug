#include "e820.hpp"
#include "bios.hpp"
#include "sort.hpp"
#include "console.hpp"

asm(".code16gcc");

E820::Entry E820::entries[E820::MAX_ENTRIES];
uint16_t E820::count;

void E820::detectMemory() {
    uint32_t continuationID = 0;

    do {
        auto& entry = entries[count];
        BIOS::RegPack args;

        args.eax = 0xe820;
        args.ebx = continuationID;
        args.ecx = sizeof entry;
        args.edx = SMAP_SIGNATURE;
        args.edi = (uint32_t)&entry;

        BIOS::RegPack output = BIOS::interrupt(0x15, args);
        if (output.eax != SMAP_SIGNATURE)
            return;

        if (output.ecx <= sizeof entry || entry.acpi & 1)
            ++count;

        continuationID = output.ebx;
    } while (continuationID != 0 && count < MAX_ENTRIES);
}

asm(".code32");

Array<E820::Entry, E820::MAX_ENTRIES> E820::map;

void E820::sanitizeMap() {
    struct ChangePoint {
        uint64_t addr;
        const Entry* entry;
    };

    Array<ChangePoint, 2 * MAX_ENTRIES> changePoints;
    for (size_t i = 0; i < count; ++i) {
        const auto& entry = entries[i];

        if (entry.length != 0) {
            changePoints.insert({ entry.base, &entry });
            changePoints.insert({ entry.base + entry.length, &entry });
        }
    }

    sort(changePoints.data(), changePoints.size(), [](const auto& p1, const auto& p2) {
        if (p1.addr == p2.addr)
            return p1.addr == p1.entry->base;

        return p1.addr < p2.addr;
    });

    Array<const Entry*, MAX_ENTRIES> overlaps;
    uint16_t prevType = 0;
    Entry newEntry;

    for (const auto& changePoint : changePoints) {
        if (changePoint.addr == changePoint.entry->base) {
            overlaps.insert(changePoint.entry);
        } else {
            size_t idx = overlaps.find(changePoint.entry);
            if (idx != overlaps.size())
                overlaps.remove(idx);
        }

        uint16_t currType = 0;
        for (const auto& entry : overlaps)
            if (entry->type > currType)
                currType = entry->type;

        if (currType != prevType) {
            if (prevType != 0 && changePoint.addr - newEntry.base != 0) {
                newEntry.length = changePoint.addr - newEntry.base;
                map.insert(newEntry);
            }

            if (currType != 0) {
                newEntry.base = changePoint.addr;
                newEntry.type = currType;
            }

            prevType = currType;
        }
    }

//    for (size_t i = 0; i < count; ++i) {
//        const auto& e = entries[i];
//        Console::printf("%.8x %.8x %d\n", (uint32_t)e.base, (uint32_t)e.length, e.type);
//    }

//    Console::print('\n');

//    for (const auto& e : map) {
//        Console::printf("%.8x %.8x %d\n", (uint32_t)e.base, (uint32_t)e.length, e.type);
//    }
}
