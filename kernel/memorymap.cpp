#include "memorymap.hpp"
#include "boot/e820.hpp"
#include "console.hpp"
#include "paging.hpp"

uint32_t MemoryMap::start;
uint32_t MemoryMap::end;
MemoryMap::HoleList MemoryMap::holes;

MemoryMap::Entry::Entry(uint64_t start, uint64_t end, bool available) :
    base(start), length(end - start), available(available) { }

MemoryMap::Entry::Entry(const E820::Entry& e) :
    base(e.base), length(e.length), available(e.isAvailable()) { }

void MemoryMap::init() {
    E820::sanitizeMap();

    Array<Entry, E820::MAX_ENTRIES * 2> entries; // 2x to save space for unlisted regions
    entries.insert(E820::map[0]);

    // Insert all E820 entries, fill up unlisted regions
    for (size_t i = 1; i < E820::map.size(); ++i) {
        const auto& entry = E820::map[i];
        const auto& prevEntry = entries.last();

        // If there's an unlisted region, insert it as non-available
        if (entry.base != prevEntry.base + prevEntry.length)
            entries.insert({ prevEntry.base + prevEntry.length, entry.base, false });

        // Discard entries above 4GiB
        if (entry.base + entry.length >= MAX_MEMORY) {
            if (entry.base != MAX_MEMORY)
                entries.insert({ entry.base, MAX_MEMORY, entry.isAvailable() });

            break;
        }

        entries.insert(entry);
    }

    // merge adjacent ranges of same type
    for (size_t i = 1; i < entries.size(); ++i) {
        auto& prev = entries[i - 1];
        auto& curr = entries[i];

        if (prev.available == curr.available) {
            prev.length += curr.length;
            entries.remove(i);
            --i;
        }
    }

    // page align all entries
    for (auto& e : entries) {
        if (e.available) {
            e.base = Paging::alignUp(e.base);
            e.length = Paging::alignDown(e.length);
        } else {
            e.base = Paging::alignDown(e.base);
            e.length = Paging::alignUp(e.length);
        }
    }

    if (!entries.last().available) {
        end = entries.last().base;
    } else {
        if (entries.last().base + entries.last().length == 0) { // if end wraps around 4GiB
            end = Paging::alignDown(0xffffffff);
        }

        end = entries.last().base + entries.last().length;
    }

    for (const auto& e : entries) {
        if (e.base >= end)
            break;

        if (!e.available)
            holes.insert(e);
    }
}

uint32_t MemoryMap::getEnd(){
    return end;
}

const MemoryMap::HoleList& MemoryMap::getHoles() {
    return holes;
}

const MemoryMap::Entry* MemoryMap::isInHole(uint32_t base, uint32_t length) {
    for (const auto& hole : holes) {
        auto holeStart    = hole.base;
        auto holeEnd      = hole.base + hole.length;
        auto addressStart = base;
        auto addressEnd   = base + length;

        if (holeStart < addressEnd && addressStart < holeEnd)
            return &hole;
    }

    return nullptr;
}
