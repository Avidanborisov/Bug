#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <stdint.h>

class Context {
public:
    struct Registers {
        uint32_t ds;                                         // data segment selector, pushed by common isr/irq handler
        uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax; // pushed by pusha
        uint32_t intNum, errorCode;                          // pushed by each isr/irq handler
        uint32_t eip, cs, eflags, esp, ss;                   // pushed by the processor when doing an interrupt
    };

    static void save()    asm("context_save");
    static void restore() asm("context_restore");
};

#endif // CONTEXT_HPP
