#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <stdint.h>
#include "console.hpp"

class Framebuffer {
public:
    static constexpr auto HEIGHT = 25;
    static constexpr auto WIDTH  = 80;

    using Monitor = uint16_t[Framebuffer::HEIGHT][Framebuffer::WIDTH];

    static constexpr uint32_t PHYSICAL_MEMORY_START = 0xb8000;
    static constexpr uint32_t PHYSICAL_MEMORY_END   = 0xb8fa0;

    static struct Cursor {
        // IO ports for cursor control
        static constexpr auto COMMAND_PORT = 0x3d4;
        static constexpr auto DATA_PORT    = 0x3d5;

        // Commands for cursor IO ports
        static constexpr auto HIGH_BYTE_COMMAND = 14;
        static constexpr auto LOW_BYTE_COMMAND  = 15;

        static void update(int x, int y); // update cursor position on screen
    } cursor;

    using Color = Console::Color;

    static uint16_t makeChar(char c, Color fg, Color bg);
    static const uint16_t BLANK; // default blank character
};

#endif // FRAMEBUFFER_HPP
