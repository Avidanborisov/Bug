#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <stdint.h>

class Context {
public:
    struct Registers {
        // data segment selector, pushed by common isr/irq handler or by the kernel when doing context switch
        uint32_t ds;

        // registers, pushed by pusha (by isr/irq handler or context switch)
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t useless;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;

        // interrupt info, pushed by each isr/irq handler
        uint32_t intNum;
        uint32_t errorCode;

        // pushed by the processor when doing an interrupt, or by the kernel when doing context switch
        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
        uint32_t esp;
        uint32_t ss;
    };
};

#endif // CONTEXT_HPP
