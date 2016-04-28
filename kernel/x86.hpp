#ifndef X86_HPP
#define X86_HPP

#include <stdint.h>
#include "gdt.hpp"
#include "idt.hpp"

class x86 {
public:
    static void out(uint16_t port, uint8_t data) asm("x86_out");
    static uint8_t inb(uint16_t port)            asm("x86_inb");
    static uint16_t inw(uint16_t port)           asm("x86_inw");

    static void lgdt(const GDT::Pointer& ptr)    asm("x86_lgdt");
    static void lidt(const IDT::Pointer& ptr)    asm("x86_lidt");

    static void sti()                            asm("x86_sti");
    static void cli()                            asm("x86_cli");
    static void hlt()                            asm("x86_hlt");

    static void interrupt(uint8_t n)             asm("x86_interrupt");

    static void invlpg(uint32_t virtualAddress)  asm("x86_invlpg");
    static void invlpg(const void* virt)         asm("x86_invlpg");

    class regs {
        template<class Base>
        struct Register {
            void operator+=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() + val;
            }

            void operator-=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() - val;
            }

            void operator*=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() * val;
            }

            void operator/=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() / val;
            }

            void operator%=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() % val;
            }

            void operator^=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() ^ val;
            }

            void operator&=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() & val;
            }

            void operator|=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() | val;
            }

            void operator<<=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() << val;
            }

            void operator>>=(uint32_t val) {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() >> val;
            }

            void operator++() {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() + 1;
            }

            void operator--() {
                auto& reg = *static_cast<Base*>(this);
                reg = reg() - 1;
            }
        };

    public:
        static struct cs : Register<cs> {
            void operator=(uint32_t val)         asm("x86_set_cs");
            uint32_t operator()()                asm("x86_get_cs");
        } cs                                     asm("x86_cs");

        static struct ds : Register<ds> {
            void operator=(uint32_t val)         asm("x86_set_ds");
            uint32_t operator()()                asm("x86_get_ds");
        } ds                                     asm("x86_ds");

        static struct es : Register<es>  {
            void operator=(uint32_t val)         asm("x86_set_es");
            uint32_t operator()()                asm("x86_get_es");
        } es                                     asm("x86_es");

        static struct fs : Register<fs>  {
            void operator=(uint32_t val)         asm("x86_set_fs");
            uint32_t operator()()                asm("x86_get_fs");
        } fs                                     asm("x86_fs");

        static struct gs : Register<gs>  {
            void operator=(uint32_t val)         asm("x86_set_gs");
            uint32_t operator()()                asm("x86_get_gs");
        } gs                                     asm("x86_gs");

        static struct ss : Register<ss>  {
            void operator=(uint32_t val)         asm("x86_set_ss");
            uint32_t operator()()                asm("x86_get_ss");
        } ss                                     asm("x86_ss");

        static struct cr0 : Register<cr0>  {
            void operator=(uint32_t val)         asm("x86_set_cr0");
            uint32_t operator()()                asm("x86_get_cr0");
        } cr0                                    asm("x86_cr0");

        static struct cr2 : Register<cr2>  {
            void operator=(uint32_t val)         asm("x86_set_cr2");
            uint32_t operator()()                asm("x86_get_cr2");
        } cr2                                    asm("x86_cr2");

        static struct cr3 : Register<cr3>  {
            void operator=(uint32_t val)         asm("x86_set_cr3");
            uint32_t operator()()                asm("x86_get_cr3");
        } cr3                                    asm("x86_cr3");

        static struct tr : Register<tr>  {
            void operator=(uint32_t val)         asm("x86_set_tr");
            uint32_t operator()()                asm("x86_get_tr");
        } tr                                     asm("x86_tr");

        static struct eip : Register<eip>  {
            uint32_t operator()()                asm("x86_get_eip");
        } eip                                    asm("x86_eip");
    };

    enum Flags {
        CF   = 1 << 0,
        PF   = 1 << 2,
        AF   = 1 << 4,
        ZF   = 1 << 6,
        SF   = 1 << 7,
        TF   = 1 << 8,
        IF   = 1 << 9,
        DF   = 1 << 10,
        OF   = 1 << 11,
        IOPL = 3 << 12,
        NT   = 1 << 14,
        RF   = 1 << 16,
        VM   = 1 << 17,
        AC   = 1 << 18,
        VIF  = 1 << 19,
        VIP  = 1 << 20,
        ID   = 1 << 21,
    };
};

#endif // X86_HPP
