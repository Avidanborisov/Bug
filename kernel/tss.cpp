#include "tss.hpp"
#include "memops.hpp"
#include "x86.hpp"

TSS TSS::tss;

void TSS::init() {
    memset(&tss, 0, sizeof tss);
    tss.ss0 = GDT::DATA_SELECTOR;
    x86::regs::tr = GDT::TSS_SELECTOR;
}

uint32_t TSS::base() {
    return reinterpret_cast<uint32_t>(&tss);
}

uint32_t& TSS::espKernel() {
    return tss.esp0;
}

uint32_t& TSS::ssKernel() {
    return tss.ss0;
}
