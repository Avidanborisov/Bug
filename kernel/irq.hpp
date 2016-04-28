#ifndef IRQ_HPP
#define IRQ_HPP

#include <stdint.h>
#include "context.hpp"

class IRQ {
public:
    static void init();

    using Handler = void(Context::Registers&);
    static void handle(uint8_t num, Handler* handler);

    static constexpr auto NUM_HANDLERS = 16;

private:
    static void commonHandler(Context::Registers& regs) asm("IRQ_commonHandler");
    static Handler* handlers[];

    class PIC {
    public:
        PIC(uint16_t commandPort, uint16_t dataPort);

        void sendCommand(uint8_t cmd) const;
        void sendData(uint8_t data) const;
        void sendEOI() const;

        uint8_t readISR() const;

    private:
        uint16_t commandPort;
        uint16_t dataPort;
    };

    static PIC master, slave;
};

#endif // IRQ_HPP
