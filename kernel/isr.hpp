#ifndef ISR_HPP
#define ISR_HPP

#include "context.hpp"

class ISR {
public:
    static void init();

    using Handler = void(const Context::Registers&);
    static void handle(uint8_t num, Handler* handler);

    static constexpr auto NUM_HANDLERS = 32;

private:
    static void commonHandler(const Context::Registers& regs) asm("isr_common_handler");
    static Handler* handlers[];
};

#endif // ISR_HPP
