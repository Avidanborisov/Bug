#include "isr.hpp"
#include "idt.hpp"
#include "assert.hpp"
#include "console.hpp"
#include "syscalls.hpp"
#include "kernel.hpp"

extern "C" void  isr0(void);
extern "C" void  isr1(void);
extern "C" void  isr2(void);
extern "C" void  isr3(void);
extern "C" void  isr4(void);
extern "C" void  isr5(void);
extern "C" void  isr6(void);
extern "C" void  isr7(void);
extern "C" void  isr8(void);
extern "C" void  isr9(void);
extern "C" void isr10(void);
extern "C" void isr11(void);
extern "C" void isr12(void);
extern "C" void isr13(void);
extern "C" void isr14(void);
extern "C" void isr15(void);
extern "C" void isr16(void);
extern "C" void isr17(void);
extern "C" void isr18(void);
extern "C" void isr19(void);
extern "C" void isr20(void);
extern "C" void isr21(void);
extern "C" void isr22(void);
extern "C" void isr23(void);
extern "C" void isr24(void);
extern "C" void isr25(void);
extern "C" void isr26(void);
extern "C" void isr27(void);
extern "C" void isr28(void);
extern "C" void isr29(void);
extern "C" void isr30(void);
extern "C" void isr31(void);

extern "C" void isr80(void);

ISR::Handler* ISR::handlers[ISR::NUM_EXCEPTIONS] = { nullptr };

void ISR::init() {
    IDT::setEntry(0 , isr0);
    IDT::setEntry(1 , isr1);
    IDT::setEntry(2 , isr2);
    IDT::setEntry(3 , isr3);
    IDT::setEntry(4 , isr4);
    IDT::setEntry(5 , isr5);
    IDT::setEntry(6 , isr6);
    IDT::setEntry(7 , isr7);
    IDT::setEntry(8 , isr8);
    IDT::setEntry(9 , isr9);
    IDT::setEntry(10, isr10);
    IDT::setEntry(11, isr11);
    IDT::setEntry(12, isr12);
    IDT::setEntry(13, isr13);
    IDT::setEntry(14, isr14);
    IDT::setEntry(15, isr15);
    IDT::setEntry(16, isr16);
    IDT::setEntry(17, isr17);
    IDT::setEntry(18, isr18);
    IDT::setEntry(19, isr19);
    IDT::setEntry(10, isr20);
    IDT::setEntry(21, isr21);
    IDT::setEntry(22, isr22);
    IDT::setEntry(23, isr23);
    IDT::setEntry(24, isr24);
    IDT::setEntry(25, isr25);
    IDT::setEntry(26, isr26);
    IDT::setEntry(27, isr27);
    IDT::setEntry(28, isr28);
    IDT::setEntry(29, isr29);
    IDT::setEntry(30, isr30);
    IDT::setEntry(31, isr31);

    IDT::setEntry(80, isr80, 3); // syscall handler
}

void ISR::handle(uint8_t num, ISR::Handler* handler) {
    assert(num < NUM_EXCEPTIONS);
    handlers[num] = handler;
}

void ISR::commonHandler(Context::Registers& regs) {
    if (regs.intNum == SYSCALL_HANDLER) {
        Syscalls::handle(regs);
        return;
    }

    if (handlers[regs.intNum])
        handlers[regs.intNum](regs);
    else
        Kernel::panic("Unhandled interrupt: %d\n", regs.intNum);
}
