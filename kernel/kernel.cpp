#include <stdint.h>
#include "kernel.hpp"
#include "framebuffer.hpp"

void Kernel::main() {
    Framebuffer::clear();

    const auto start = (int)Framebuffer::Color::BLACK;
    const auto end = (int)Framebuffer::Color::WHITE;

    // nicely print all letters with all colors :)
    int letter = 0;
    for (int fg = end; fg >= start; --fg) {
        for (int bg = start; bg <= end; ++bg) {
            if (fg == bg)
                continue;

            Framebuffer::putchar(letter + 'A', (Framebuffer::Color)fg, (Framebuffer::Color)bg);
            letter = (letter + 1) % 26;
        }
    }
}
