#ifndef TSS_HPP
#define TSS_HPP

#include "gdt.hpp"

class TSS {
public:
    static void init();
    static uint32_t base();

    static uint32_t& espKernel();
    static uint32_t& ssKernel();

private:
    static TSS tss;

    uint32_t prevTSS;
    uint32_t esp0, ss0, esp1, ss1, esp2, ss2;
    uint32_t cr3, eip, eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomapBase;
};

static_assert(sizeof(TSS) == 104, "The (single) Task State Segment entry is 104 bytes long");

#endif // TSS_HPP
