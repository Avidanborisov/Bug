#include "irq.hpp"
#include "idt.hpp"
#include "x86.hpp"
#include "assert.hpp"
#include "console.hpp"

extern "C" void  irq0(void);
extern "C" void  irq1(void);
extern "C" void  irq2(void);
extern "C" void  irq3(void);
extern "C" void  irq4(void);
extern "C" void  irq5(void);
extern "C" void  irq6(void);
extern "C" void  irq7(void);
extern "C" void  irq8(void);
extern "C" void  irq9(void);
extern "C" void irq10(void);
extern "C" void irq11(void);
extern "C" void irq12(void);
extern "C" void irq13(void);
extern "C" void irq14(void);
extern "C" void irq15(void);

IRQ::Handler* IRQ::handlers[IRQ::NUM_HANDLERS] = { nullptr };

IRQ::PIC IRQ::master(0x20, 0x21);
IRQ::PIC IRQ::slave(0xa0, 0xa1);

void IRQ::init() {
    constexpr auto NEW_IRQ_OFFSET = 32;

    // Restart the both PICs
    master.sendCommand(0x11);
    slave.sendCommand(0x11);

    // Make PIC1 and PIC2 signal to ISR32..47
    master.sendData(NEW_IRQ_OFFSET);
    slave.sendData(NEW_IRQ_OFFSET + 8);

    master.sendData(0x4); // tell PIC1 (Master) that there is a slave PIC (PIC2) at IRQ2 (0000 0100)
    slave.sendData(0x2);  // tell Slave PIC its cascade identity (0000 0010)

    // 8086 mode for both PICs
    master.sendData(0x1);
    slave.sendData(0x1);

    // Activate all IRQs in both PICs
    master.sendData(0);
    slave.sendData(0);

    IDT::setEntry(NEW_IRQ_OFFSET + 0 , irq0);
    IDT::setEntry(NEW_IRQ_OFFSET + 1 , irq1);
    IDT::setEntry(NEW_IRQ_OFFSET + 2 , irq2);
    IDT::setEntry(NEW_IRQ_OFFSET + 3 , irq3);
    IDT::setEntry(NEW_IRQ_OFFSET + 4 , irq4);
    IDT::setEntry(NEW_IRQ_OFFSET + 5 , irq5);
    IDT::setEntry(NEW_IRQ_OFFSET + 6 , irq6);
    IDT::setEntry(NEW_IRQ_OFFSET + 7 , irq7);
    IDT::setEntry(NEW_IRQ_OFFSET + 8 , irq8);
    IDT::setEntry(NEW_IRQ_OFFSET + 9 , irq9);
    IDT::setEntry(NEW_IRQ_OFFSET + 10, irq10);
    IDT::setEntry(NEW_IRQ_OFFSET + 11, irq11);
    IDT::setEntry(NEW_IRQ_OFFSET + 12, irq12);
    IDT::setEntry(NEW_IRQ_OFFSET + 13, irq13);
    IDT::setEntry(NEW_IRQ_OFFSET + 14, irq14);
    IDT::setEntry(NEW_IRQ_OFFSET + 15, irq15);
}

void IRQ::handle(uint8_t num, IRQ::Handler* handler) {
    assert(num < NUM_HANDLERS);
    handlers[num] = handler;
}

void IRQ::commonHandler(Context::Registers& regs) {
    // If the IRQ is on the slave PIC, send EOI to it
    if (regs.intNum >= 8)
        slave.sendEOI();

    // Handle spurious IRQ7
    if (regs.intNum == 7 && !(master.readISR() & 0x80))
        return;

    // Send EOI to the master PIC
    master.sendEOI();

    if (handlers[regs.intNum])
        handlers[regs.intNum](regs);
    else
        Console::printf("Received IRQ: %d\n", regs.intNum);
}

IRQ::PIC::PIC(uint16_t commandPort, uint16_t dataPort) :
    commandPort(commandPort), dataPort(dataPort) { }

void IRQ::PIC::sendCommand(uint8_t cmd) const {
    x86::out(commandPort, cmd);
}

void IRQ::PIC::sendData(uint8_t data) const {
    x86::out(dataPort, data);
}

void IRQ::PIC::sendEOI() const {
    sendCommand(0x20);
}

uint8_t IRQ::PIC::readISR() const {
    sendCommand(0x0b);
    return x86::inb(commandPort);
}
