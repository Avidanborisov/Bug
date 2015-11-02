#include "gdt.hpp"
#include "x86.hpp"

GDT::Entry GDT::descriptors[] = {
    // limit is 20 bytes, and granularity is 1 (4K blocks) so
    // limit = 0xffffffff (entire memory) eventually
    { 0, 0         , 0, 0     }, // null descriptor
    { 0, 0x000fffff, 0, true  }, // kernel code segment
    { 0, 0x000fffff, 0, false }, // kernel data segment
    { 0, 0x000fffff, 3, true  }, // user code segment
    { 0, 0x000fffff, 3, false }, // user data segment
};

const uint16_t GDT::CODE_SELECTOR      = 1 * sizeof(GDT::Entry);
const uint16_t GDT::DATA_SELECTOR      = 2 * sizeof(GDT::Entry);
const uint16_t GDT::USER_CODE_SELECTOR = 3 * sizeof(GDT::Entry);
const uint16_t GDT::USER_DATA_SELECTOR = 4 * sizeof(GDT::Entry);

void GDT::init() {
    Pointer ptr(descriptors, sizeof descriptors - 1);
    x86::lgdt(ptr);

    x86::regs::cs = CODE_SELECTOR;
    x86::regs::ds = DATA_SELECTOR;
    x86::regs::es = DATA_SELECTOR;
    x86::regs::fs = DATA_SELECTOR;
    x86::regs::gs = DATA_SELECTOR;
    x86::regs::ss = DATA_SELECTOR;
}

GDT::Entry::Entry(uint32_t base, uint32_t limit, uint8_t ring, bool isExec) {
    baseLow  = base & 0xffff;
    baseMid  = (base >> 16) & 0xff;
    baseHigh = (base >> 24) & 0xff;

    limitLow  = limit & 0xffff;
    limitHigh = (limit >> 16) & 0x000f;

    // Access byte
    accessed  = false;  // this byte will be set to true by the processor when the segment it accessed; initially it isn't.
    rw        = true;   // always make a code segment readable or a data segment writable
    direction = false;  // allow code in code segments to only execute in the ring, or make data segments grow up (i.e. normal behavior)
    exec      = isExec; // if true, the entry specifies a code segment, otherwise a data segment
    type      = 1;      // always 1 for GDT descriptors
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
