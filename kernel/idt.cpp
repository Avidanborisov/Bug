#include "idt.hpp"
#include "gdt.hpp"
#include "x86.hpp"

IDT::Entry IDT::descriptors[IDT::NUM_ENTRIES] = { };

void IDT::init() {
    Pointer ptr(descriptors, sizeof descriptors - 1);
    x86::lidt(ptr);
}

void IDT::setEntry(uint8_t index, IDT::Handler* func, uint8_t ring) {
    descriptors[index] = { func, ring };
}

IDT::Entry::Entry(IDT::Handler* func, uint8_t ring) {
    auto base = (uint32_t)func;

    baseLow  = base & 0xffff;
    baseHigh = (base >> 16) & 0xffff;

    selector = GDT::CODE_SELECTOR;
    zero     = 0;

    // Flags byte
    type           = 0xE;  // interrupt gate
    storageSegment = 0;    // 0 for interrupt gates
    privilege      = ring;
    present        = true; // make new interrupt entry present
}

IDT::Pointer::Pointer(const IDT::Entry* base, uint16_t size) :
    size(size),
    base((uint32_t)base)
{ }
