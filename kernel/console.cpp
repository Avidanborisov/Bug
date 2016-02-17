#include "console.hpp"
#include "assert.hpp"

void Console::init() {
    Framebuffer::clear();
    print("                            Welcome to Bug OS!\n", Console::Color::CYAN);
}

void Console::print(char c, Console::Color fg, Console::Color bg) {
    Framebuffer::putchar(c, fg, bg);
}

void Console::print(const char* s, Console::Color fg, Console::Color bg) {
    while (*s != '\0')
        print(*s++, fg, bg);
}

void Console::print(const String& s, Console::Color fg, Console::Color bg) {
    print(s.data(), fg, bg);
}

void Console::print(bool b, Console::Color fg, Console::Color bg) {
    print(b ? "true" : "false", fg, bg);
}

void Console::print(uint32_t num, int base, int minWidth, bool zeroPad, Color fg, Color bg) {
    static const char* CHARMAP = "0123456789abcdefghijklmnopqrstuvwxyz";    
    assert(base >= 2 && base <= 36);

    int i = 0;
    int digits[32]; // max 32 digits for binary base

    do {
        digits[i++] = num % base;
        num /= base;
    } while (num > 0);

    while (--minWidth >= i)
        print(zeroPad ? '0' : ' ', fg, bg);

    while (--i >= 0)
        print(CHARMAP[digits[i]], fg, bg);
}

void Console::print(int32_t num, int base, int minWidth, bool zeroPad, Console::Color fg, Console::Color bg) {
    if (num < 0) {
        print('-', fg, bg);
        print(static_cast<uint32_t>(-num), base, minWidth, zeroPad, fg, bg);
    } else {
        print(static_cast<uint32_t>(num), base, minWidth, zeroPad, fg, bg);
    }
}

void Console::print(const void* addr, int base, int minWidth, bool zeroPad, Console::Color fg, Console::Color bg) {
    print(reinterpret_cast<uint32_t>(addr), base, minWidth, zeroPad, fg, bg);
}

void Console::print(unsigned num, int base, int minWidth, bool zeroPad, Console::Color fg, Console::Color bg) {
    print(static_cast<uint32_t>(num), base, minWidth, zeroPad, fg, bg);
}

void Console::print(int num, int base, int minWidth, bool zeroPad, Console::Color fg, Console::Color bg) {
    print(static_cast<int32_t>(num), base, minWidth, zeroPad, fg, bg);
}

void Console::printfImpl(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(Color::WHITE, Color::BLACK, fmt, ap);
}

void Console::printfImpl(Color fg, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fg, Color::BLACK, fmt, ap);
}

void Console::printfImpl(Color fg, Color bg, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fg, bg, fmt, ap);
}

void Console::vprintf(Console::Color fg, const char* fmt, va_list ap) {
    vprintf(fg, Color::BLACK, fmt, ap);
}

void Console::vprintf(Console::Color fg, Console::Color bg, const char* fmt, va_list ap) {
    int minWidth = -1;
    bool zeroPad = false;

    auto printNum = [&](auto num, int base) {
        print(num, base, minWidth, zeroPad, fg, bg);
        minWidth = -1;
    };

    int i = 0;
    while (char c = fmt[i++]) {
        if (minWidth == -1) {
            if (c != '%') {
                print(c, fg, bg);
                continue;
            }

            c = fmt[i++];
        }

        switch (c) {
            case '%':
                print("%%", fg, bg);
                break;

            case 'c':
                print((char)va_arg(ap, int), fg, bg);
                break;

            case 't':
                print((bool)va_arg(ap, int), fg, bg);
                break;

            case 's':
                print(va_arg(ap, const char*), fg, bg);
                break;

            case 'u':
                printNum(va_arg(ap, uint32_t), 10);
                break;

            case 'x':
                printNum(va_arg(ap, uint32_t), 16);
                break;

            case 'b':
                printNum(va_arg(ap, uint32_t), 2);
                break;

            case 'd':
                printNum(va_arg(ap, int32_t), 10);
                break;

            case 'p':
                printNum(va_arg(ap, const void*), 16);
                break;

            case '.':
                minWidth = 0;
                zeroPad = false;

                for (int j = i; (c = fmt[j]) != '\0'; ++j) {
                    if (c >= '0' && c <= '9') {
                        minWidth = 10 * minWidth + c - '0';

                        if (c == '0' && j == i)
                            zeroPad = true;
                    } else {
                        if (c == 'u' || c == 'x' || c == 'b' || c == 'd' || c == 'p')
                            i = j;

                        break;
                    }
                }

                if (minWidth == -1)
                    print("%.", fg, bg);

                break;

            default:
                print('%', fg, bg);
                print(c, fg, bg);
                break;
        }
    }
}

