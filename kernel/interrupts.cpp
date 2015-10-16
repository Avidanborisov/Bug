#include "interrupts.hpp"
#include "idt.hpp"
#include "isr.hpp"
#include "irq.hpp"
#include "x86.hpp"

void Interrupts::init() {
    IDT::init();
    ISR::init();
    IRQ::init();
    x86::sti();
}
