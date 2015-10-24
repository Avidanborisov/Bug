#include "memorymap.hpp"
#include "boot/e820.hpp"
#include "console.hpp"

Array<MemoryMap::Entry, E820::MAX_ENTRIES * 2> MemoryMap::entries;

MemoryMap::Entry::Entry(uint64_t base, uint64_t length, bool available) :
    base(base), length(length), available(available) { }

MemoryMap::Entry::Entry(const E820::Entry& e) :
    base(e.base), length(e.length), available(e.isAvailable()) { }

void MemoryMap::init() {
    constexpr uint64_t MEMORY_END = 0x100000000; // 4GiB

    E820::sanitizeMap();
    entries.insert(E820::map[0]);

    // Insert all E820 entries, fill up unlisted regions
    for (size_t i = 1; i < E820::map.size(); ++i) {
        const auto& entry = E820::map[i];
        const auto& prevEntry = entries.last();

        if (entry.base != prevEntry.base + prevEntry.length) {
            Entry unlisted;
            unlisted.base = prevEntry.base + prevEntry.length;
            unlisted.length = entry.base - unlisted.base;
            unlisted.available = false;

            entries.insert(unlisted);
        }

        // Discard entries above 4GiB
        if (entry.base + entry.length >= MEMORY_END) {
            if (entry.base != MEMORY_END)
                entries.insert({ entry.base, MEMORY_END - entry.base, entry.isAvailable() });

            break;
        }

        entries.insert(entry);
    }


    // If E820 didn't cover entire 4GiB memory space, complete the map
    if (entries.last().base + entries.last().length != 0) {
        entries.insert({
            entries.last().base + entries.last().length,
            MEMORY_END - entries.last().base - entries.last().length,
            false
        });
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

    for (const auto& e : entries) {
        Console::printf("%.8x %.8x %t\n", e.base, e.length, e.available);
    }
}
