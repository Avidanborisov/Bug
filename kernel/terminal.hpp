#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "framebuffer.hpp"
#include "console.hpp"
#include "array.hpp"

class Terminal {
public:
    static constexpr auto NUM_TERMINALS = 12;

    void clear();
    void putchar(char c, Console::Color fg = Console::Color::WHITE, Console::Color bg = Console::Color::BLACK);
    void print(const char* s, Console::Color fg = Console::Color::WHITE, Console::Color bg = Console::Color::BLACK);

    static void setActive(int tty);
    static int getActive();
    static Terminal& get(int tty);

private:
    void clear(Framebuffer::Monitor& monitor);
    void scroll();

    struct {
        uint8_t x, y;
    } cursor = { 0, 0 };

    void updateCursor();

    size_t id() const;
    Framebuffer::Monitor& monitor();

    Framebuffer::Monitor* backend = nullptr;
    Framebuffer::Monitor& virtualMonitor();

    static size_t active;
    static Terminal terminals[];
    static Framebuffer::Monitor& physicalMonitor;
};

#endif // TERMINAL_HPP
