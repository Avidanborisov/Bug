#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "context.hpp"

class Keyboard {
public:
    static constexpr auto SCAN_PORT = 0x60;
    static constexpr auto SCAN_CODES = 120;

    static const char SCAN_CODE_RELEASED    = 0x80;
    static const char SCAN_CODE_ENTER       = 0x1c;
    static const char SCAN_CODE_BACKSPACE   = 0x0e;
    static const char SCAN_CODE_UP          = 0x48;
    static const char SCAN_CODE_DOWN        = 0x50;
    static const char SCAN_CODE_LEFT_SHIFT  = 0x2a;
    static const char SCAN_CODE_RIGHT_SHIFT = 0x36;

    static const char SCAN_CODE_F1  = 0x3b;
    static const char SCAN_CODE_F2  = 0x3c;
    static const char SCAN_CODE_F3  = 0x3e;
    static const char SCAN_CODE_F4  = 0x3d;
    static const char SCAN_CODE_F5  = 0x3f;
    static const char SCAN_CODE_F6  = 0x40;
    static const char SCAN_CODE_F7  = 0x41;
    static const char SCAN_CODE_F8  = 0x42;
    static const char SCAN_CODE_F9  = 0x43;
    static const char SCAN_CODE_F10 = 0x44;
    static const char SCAN_CODE_F11 = 0x57;
    static const char SCAN_CODE_F12 = 0x58;

    static void install();

private:
    static void handler(Context::Registers& regs);

    static int getFunction(int scan);

    static bool shifted;
    static const char ASCII_MAP[];
    static const char SHIFTED_ASCII_MAP[];
};

#endif // KEYBOARD_HPP
