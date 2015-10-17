#include "timer.hpp"
#include "x86.hpp"
#include "irq.hpp"
#include "context.hpp"
#include "console.hpp"

void Timer::disable() {
    x86::out(PIT_COMMAND_PORT, 0x31);
}

void Timer::start(uint32_t freq) {
    IRQ::handle(0, [](const Context::Registers&) {
        static uint32_t tick = 0;
        Console::printf("Tick: %d\n", ++tick);
    });

    uint32_t divisor = PIT_FREQUENCY / freq;

    x86::out(PIT_COMMAND_PORT, 0x36);
    x86::out(PIT_CHANNEL0_PORT, divisor & 0xff);
    x86::out(PIT_CHANNEL0_PORT, divisor >> 8);
}
