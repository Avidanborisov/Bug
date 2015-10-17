#ifndef TIMER_HPP
#define TIMER_HPP

#include <stdint.h>

class Timer {
public:
    static void disable();
    static void start(uint32_t freq);

private:
    static constexpr auto PIT_COMMAND_PORT  = 0x43;
    static constexpr auto PIT_CHANNEL0_PORT = 0x40;
    static constexpr auto PIT_FREQUENCY     = 1193180;
};

#endif // TIMER_HPP
