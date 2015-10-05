#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <stdint.h>

class Framebuffer {
public:
    enum class Color {
        BLACK         = 0,
        BLUE          = 1,
        GREEN         = 2,
        CYAN          = 3,
        RED           = 4,
        MAGENTA       = 5,
        BROWN         = 6,
        LIGHT_GREY    = 7,
        DARK_GREY     = 8,
        LIGHT_BLUE    = 9,
        LIGHT_GREEN   = 10,
        LIGHT_CYAN    = 11,
        LIGHT_RED     = 12,
        LIGHT_MAGENTA = 13,
        LIGHT_BROWN   = 14,
        WHITE         = 15,
    };

    static constexpr auto HEIGHT = 25;
    static constexpr auto WIDTH  = 80;

    // clear screen
    static void clear();

    // put single char on the screen, dealing with special characters, scrolling, etc..
    static void putchar(char c, Color fg = Color::WHITE, Color bg = Color::BLACK);

private:
    static struct Cursor {
        uint8_t x, y;

        // IO ports for cursor control
        static constexpr auto COMMAND_PORT = 0x3d4;
        static constexpr auto DATA_PORT    = 0x3d5;

        // Commands for cursor IO ports
        static constexpr auto HIGH_BYTE_COMMAND = 14;
        static constexpr auto LOW_BYTE_COMMAND  = 15;

        void update() const; // update cursor position on screen
    } cursor;

    static constexpr uint16_t makeChar(char c, Color fg, Color bg);
    static const uint16_t BLANK; // default blank character

    // scroll lines up when reaching the end of the screen
    static void scroll();
};

#endif // FRAMEBUFFER_HPP
