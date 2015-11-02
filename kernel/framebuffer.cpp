#include "framebuffer.hpp"
#include "x86.hpp"

using Monitor = uint16_t[Framebuffer::HEIGHT][Framebuffer::WIDTH];
static auto& monitor = *reinterpret_cast<Monitor*>(Framebuffer::PHYSICAL_MEMORY_START);

Framebuffer::Cursor Framebuffer::cursor = { 0, 0 };

void Framebuffer::Cursor::update() const {
    uint16_t location = y * WIDTH + x;
    x86::out(COMMAND_PORT, HIGH_BYTE_COMMAND);
    x86::out(DATA_PORT, location >> 8);
    x86::out(COMMAND_PORT, LOW_BYTE_COMMAND);
    x86::out(DATA_PORT, location & 0xff);
}

constexpr uint16_t Framebuffer::makeChar(char c, Framebuffer::Color fg, Framebuffer::Color bg) {
    return c | static_cast<uint16_t>(bg) << 12 | static_cast<uint16_t>(fg) << 8;
}

const uint16_t Framebuffer::BLANK = Framebuffer::makeChar(' ', Framebuffer::Color::WHITE, Framebuffer::Color::BLACK);

void Framebuffer::clear() {
    for (auto& row : monitor)
        for (auto& cell : row)
            cell = BLANK;

    cursor = { 0, 0 };
    cursor.update();
}

void Framebuffer::scroll() {
    if (cursor.y < Framebuffer::HEIGHT)
        return;

    // move all lines upwords
    for (auto i = 1; i < Framebuffer::HEIGHT; ++i)
        for (auto j = 0; j < Framebuffer::WIDTH; ++j)
            monitor[i - 1][j] = monitor[i][j];

    // clear last line
    for (auto i = 0; i < Framebuffer::WIDTH; ++i)
        monitor[Framebuffer::HEIGHT - 1][i] = BLANK;

    // set y offset to last line
    cursor.y = Framebuffer::HEIGHT - 1;
}

void Framebuffer::putchar(char c, Framebuffer::Color fg, Framebuffer::Color bg) {
    if (c == '\b' && cursor.x != 0) {
        --cursor.x;
    } else if (c == '\t') {
        cursor.x = (cursor.x / 8 + 1) * 8;
    } else if (c == '\r') {
        cursor.x = 0;
    } else if (c == '\n') {
        cursor.x = 0;
        ++cursor.y;
    } else if (c >= ' ') { // printable
        monitor[cursor.y][cursor.x] = makeChar(c, fg, bg);
        ++cursor.x;
    }

    if (cursor.x >= Framebuffer::WIDTH) {
        cursor.x = 0;
        ++cursor.y;
    }

    scroll();
    cursor.update();
}
