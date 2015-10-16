#ifndef X86_HPP
#define X86_HPP

#include <stdint.h>
#include "gdt.hpp"

class x86 {
public:
    static void out(uint16_t port, uint8_t data) asm("x86_out");
    static uint8_t inb(uint16_t port)            asm("x86_inb");
    static uint16_t inw(uint16_t port)           asm("x86_inw");

    static void lgdt(const GDT::Pointer& ptr)    asm("x86_lgdt");

    struct regs {
        static struct {
            void operator=(uint32_t val)         asm("x86_set_cs");
            uint32_t operator()()                asm("x86_get_cs");
        } cs                                     asm("x86_cs");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_ds");
            uint32_t operator()()                asm("x86_get_ds");
        } ds                                     asm("x86_ds");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_es");
            uint32_t operator()()                asm("x86_get_es");
        } es                                     asm("x86_es");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_fs");
            uint32_t operator()()                asm("x86_get_fs");
        } fs                                     asm("x86_fs");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_gs");
            uint32_t operator()()                asm("x86_get_gs");
        } gs                                     asm("x86_gs");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_ss");
            uint32_t operator()()                asm("x86_get_ss");
        } ss                                     asm("x86_ss");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_cr0");
            uint32_t operator()()                asm("x86_get_cr0");
        } cr0                                    asm("x86_cr0");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_cr2");
            uint32_t operator()()                asm("x86_get_cr2");
        } cr2                                    asm("x86_cr2");

        static struct {
            void operator=(uint32_t val)         asm("x86_set_cr3");
            uint32_t operator()()                asm("x86_get_cr3");
        } cr3                                    asm("x86_cr3");
    };
};

#endif // X86_HPP