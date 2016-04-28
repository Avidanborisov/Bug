#include "gdt.hpp"
#include "x86.hpp"
#include "tss.hpp"

GDT::Entry GDT::descriptors[] = {
    // limit is 20 bytes, and granularity is 1 (4K blocks) so
    // limit = 0xffffffff (entire memory) eventually
    { 0, 0         , 0, 0     }, // null descriptor
    { 0, 0x000fffff, 0, true  }, // kernel code segment
    { 0, 0x000fffff, 0, false }, // kernel data segment
    { 0, 0x000fffff, 3, true  }, // user code segment
    { 0, 0x000fffff, 3, false }, // user data segment
    { TSS::base(),   3        }  // TSS descriptor
};

constexpr GDT::Selector::Selector(uint16_t index, uint8_t ring) :
    selector(index * sizeof(GDT::Entry) | ring) { }

const GDT::Selector GDT::CODE_SELECTOR(1, 0);
const GDT::Selector GDT::DATA_SELECTOR(2, 0);
const GDT::Selector GDT::USER_CODE_SELECTOR(3, 3);
const GDT::Selector GDT::USER_DATA_SELECTOR(4, 3);
const GDT::Selector GDT::TSS_SELECTOR(5, 3);

void GDT::init() {
    Pointer ptr(descriptors, sizeof descriptors - 1);
    x86::lgdt(ptr);

    x86::regs::cs = CODE_SELECTOR;
    x86::regs::ds = DATA_SELECTOR;
    x86::regs::es = DATA_SELECTOR;
    x86::regs::fs = DATA_SELECTOR;
    x86::regs::gs = DATA_SELECTOR;
    x86::regs::ss = DATA_SELECTOR;

    TSS::init();
}

GDT::Entry::Entry(uint32_t base, uint32_t limit, uint8_t ring, bool isExec) : // Normal GDT descriptor constructor
    Entry(base, limit, ring) {

    // Access byte
    accessed  = false;  // this byte will be set to true by the processor when the segment it accessed; initially it isn't.
    rw        = true;   // always make a code segment readable or a data segment writable
    exec      = isExec; // if true, the entry specifies a code segment, otherwise a data segment
    type      = 1;      // always 1 for GDT descriptors
}

GDT::Entry::Entry(uint32_t base, uint8_t ring) : // TSS descriptor constructor
    Entry(base, base + sizeof(TSS), ring) {

    // Access byte
    accessed  = 1; // always 1 for TSS descriptors
    rw        = 0; // specifies 'busy' bit for TSS descriptors. should be 0 on init.
    exec      = 1; // always 1 for TSS descriptors
    type      = 0; // always 0 for TSS descriptors
}

GDT::Entry::Entry(uint32_t base, uint32_t limit, uint8_t ring) { // Common descriptor constructor
    baseLow  = base & 0xffff;
    baseMid  = (base >> 16) & 0xff;
    baseHigh = (base >> 24) & 0xff;

    limitLow  = limit & 0xffff;
    limitHigh = (limit >> 16) & 0x000f;

    // Access byte
    direction = false;  // allow code in code segments to only execute in the ring, or make data segments grow up (i.e. normal behavior)
    privilege = ring;   // ring level
    present   = true;   // segment is always present

    // Flags byte
    always0     = 0;
    operandSize = 1; // 1 for 32-bit code
    granularity = 1; // 1 for limit in KB units
}

GDT::Pointer::Pointer(const GDT::Entry* base, uint16_t size) :
    size(size),
    base(reinterpret_cast<uint32_t>(base))
{ }
