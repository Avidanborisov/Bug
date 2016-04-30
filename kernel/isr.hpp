#ifndef ISR_HPP
#define ISR_HPP

#include "context.hpp"

class ISR {
public:
    static void init();

    using Handler = void(Context::Registers&);
    static void handle(uint8_t num, Handler* handler);

    static constexpr auto SYSCALL_HANDLER = 80;
    static constexpr auto NUM_EXCEPTIONS  = 32;

private:
    static void commonHandler(Context::Registers& regs) asm("ISR_commonHandler");
    static Handler* handlers[];

    static const char* exceptions[];
};

#endif // ISR_HPP
