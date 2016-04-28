#include "framebuffer.hpp"
#include "x86.hpp"

void Framebuffer::Cursor::update(int x, int y) {
    uint16_t location = y * WIDTH + x;
    x86::out(COMMAND_PORT, HIGH_BYTE_COMMAND);
    x86::out(DATA_PORT, location >> 8);
    x86::out(COMMAND_PORT, LOW_BYTE_COMMAND);
    x86::out(DATA_PORT, location & 0xff);
}

uint16_t Framebuffer::makeChar(char c, Framebuffer::Color fg, Framebuffer::Color bg) {
    return c | static_cast<uint16_t>(bg) << 12 | static_cast<uint16_t>(fg) << 8;
}

const uint16_t Framebuffer::BLANK = Framebuffer::makeChar(' ', Framebuffer::Color::WHITE, Framebuffer::Color::BLACK);
