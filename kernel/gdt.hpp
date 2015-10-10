#ifndef GDT_HPP
#define GDT_HPP

#include <stdint.h>

class GDT {
public:
    static void init();

    static const uint32_t CODE_SELECTOR;
    static const uint32_t DATA_SELECTOR;
    static const uint32_t USER_CODE_SELECTOR;
    static const uint32_t USER_DATA_SELECTOR;

private:
    // GDT Entry:
    //
    //  31                           16 15                            0
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    // |          Base 0:15            |           Limit 0:15          |
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //
    //  63           56 55   52 51   48 47           40 39           32
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    // |     Base      | Flags | Limit |  Access Byte  |   Base 16:23  |
    // |     24:31     |G|O| 0 | 16:19 |P|Prv|T|E|D|R|A|               |
    // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    static class Entry {
    public:
        Entry(uint32_t base, uint32_t limit, uint8_t ring, bool isExec);

    private:
        uint16_t limitLow;
        uint16_t baseLow;
        uint8_t baseMid;

        // Access byte
        uint8_t accessed  : 1; // is segment being accessed?
        uint8_t rw        : 1; // is code segment readable? or is data segment writable?
        uint8_t direction : 1; // always 0 - segment grows up or code can be executed only in ring
        uint8_t exec      : 1; // exec bit (can code be executed in this segment?)
        uint8_t type      : 1; // descriptor type
        uint8_t privilege : 2; // ring level
        uint8_t present   : 1; // is segment present?

        uint8_t limitHigh : 4;

        // Flags:
        uint8_t always0     : 2; // user-defined and reserved bit - always 0
        uint8_t operandSize : 1; // 0 for 16-bit code, 1 for 32-bit code.
        uint8_t granularity : 1; // 0 for limit in byte units, 1 for limit in KB units.

        uint8_t baseHigh;
    } descriptors[];

    static_assert(sizeof(Entry) == 8, "GDT Entry is 8 bytes long");

    // GDT Entry pointer
    class [[gnu::packed]] Pointer {
    public:
        Pointer(const Entry* base, uint16_t size);

    private:
        uint16_t size;
        uint32_t base;
    };

    static_assert(sizeof(Pointer) == 6, "GDT Pointer is 6 bytes long");

    friend class x86; // x86::lgdt needs GDT::Pointer
};

#endif // GDT_HPP
