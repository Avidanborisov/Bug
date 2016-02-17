#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <stdint.h>
#include <stdarg.h>
#include "framebuffer.hpp"
#include "string.hpp"

namespace Detail {
    template<class T>
    struct Value {
        using type = const T&;
    };

    template<class T>
    inline typename Value<T>::type value(const T& x) {
        return x;
    }

    template<>
    struct Value<String> {
        using type = const char*;
    };

    template<>
    inline const char* value<String>(const String& s) {
        return s.data();
    }
}

class Console {
public:
    using Color = Framebuffer::Color;

    static void init();

    static void print(char c,          Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(const char* s,   Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(const String& s, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(bool b,          Color fg = Color::WHITE, Color bg = Color::BLACK);

    static void print(uint32_t num, int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(int32_t num,  int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(const void*,  int base = 16, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);

    static void print(unsigned num, int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);
    static void print(int      num, int base = 10, int minWidth = 0, bool zeroPad = false, Color fg = Color::WHITE, Color bg = Color::BLACK);    

    static void vprintf(Color fg, const char* fmt, va_list ap);
    static void vprintf(Color fg, Color bg, const char* fmt, va_list ap);

    template<class... Args>
    static void printf(const Args&... args) {
        printfImpl(Detail::value(args)...);
    }

private:
    static void printfImpl(const char* fmt, ...);
    static void printfImpl(Color fg, const char* fmt, ...);
    static void printfImpl(Color fg, Color bg, const char* fmt, ...);
};

#endif // CONSOLE_HPP
