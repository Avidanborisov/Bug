#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <stdint.h>
#include <stdarg.h>
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
        YELLOW        = 14,
        WHITE         = 15,
    };

    static void clear();

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

    static char getChar();
    static String getLine();

    static bool hasInput();
    static void setPosition(int x, int y);
    static void dontPrint(bool status);

    static constexpr char UP_KEY    = 23;
    static constexpr char LEFT_KEY  = 24;
    static constexpr char DOWN_KEY  = 25;
    static constexpr char RIGHT_KEY = 26;

private:
    static void printfImpl(const char* fmt, ...);
    static void printfImpl(Color fg, const char* fmt, ...);
    static void printfImpl(Color fg, Color bg, const char* fmt, ...);
};

#endif // CONSOLE_HPP
