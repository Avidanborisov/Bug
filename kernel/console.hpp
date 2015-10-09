#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <stdint.h>
#include <stdarg.h>
#include "framebuffer.hpp"

class Console {
public:
    using Color = Framebuffer::Color;

    static void init();

    static void print(char c,        Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(const char* s, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(bool b,        Color fg = Color::WHITE, Color bg = Color::BLACK);

    static void print(uint32_t num, int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(int32_t num,  int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(const void*,  int base = 16, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);

    static void print(unsigned num, int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(int      num, int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);

    static void printf(const char* fmt, ...);
    static void printf(Color fg, const char* fmt, ...);
    static void printf(Color fg, Color bg, const char* fmt, ...);

private:
    static void vprintf(Color fg, Color bg, const char* fmt, va_list ap);
};

#endif // CONSOLE_HPP
