#ifndef IDT_HPP
#define IDT_HPP

#include <stdint.h>

class IDT {
public:
    static void init();

    using Handler = void(void);
    static void setEntry(uint8_t index, Handler* func, uint8_t ring = 0);

    static constexpr auto NUM_ENTRIES = 256;

private:
    // IDT Entry:
    //
    //  31                           16 15                            0
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    // |           Selector            |           Base 0:15           |
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

    //  63                           48 47           40 39           32
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    // |          Base 31:16           |     Flags     |      Zero     |
    // |                               |P|Prv|S| Type  |               |
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    static class Entry {
    public:
        Entry() = default;
        Entry(Handler* func, uint8_t ring);

    private:
        uint16_t baseLow;
        uint16_t selector;
        uint8_t zero;

        // Flags byte
        uint8_t type           : 4; // gate type
        uint8_t storageSegment : 1; // storage segment
        uint8_t privilege      : 2; // ring level
        uint8_t present        : 1; // is interrupt present.

        uint16_t baseHigh;
    } descriptors[];

    static_assert(sizeof(Entry) == 8, "IDT Entry is 8 bytes long");

    // IDT Entry pointer
    class [[gnu::packed]] Pointer {
    public:
        Pointer(const Entry* base, uint16_t size);

    private:
        uint16_t size;
        uint32_t base;
    };

    static_assert(sizeof(Pointer) == 6, "IDT Pointer is 6 bytes long");

    friend class x86; // x86::lidt needs GDT::Pointer
};

#endif // IDT_HPP
