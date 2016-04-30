#include "keyboard.hpp"
#include "irq.hpp"
#include "x86.hpp"
#include "console.hpp"
#include "terminal.hpp"
#include "scheduler.hpp"

bool Keyboard::shifted = false;

const char Keyboard::ASCII_MAP[Keyboard::SCAN_CODES] = {
    0   , // NUL
    27  , // ESC
    '1' ,
    '2' ,
    '3' ,
    '4' ,
    '5' ,
    '6' ,
    '7' ,
    '8' ,
    '9' ,
    '0' ,
    '-' ,
    '=' ,
    '\b', // Backspace
    '\t', // Tab
    'q' ,
    'w' ,
    'e' ,
    'r' ,
    't' ,
    'y' ,
    'u' ,
    'i' ,
    'o' ,
    'p' ,
    '[' ,
    ']' ,
    '\n', // Enter key
    0   , // Ctrl
    'a' ,
    's' ,
    'd' ,
    'f' ,
    'g' ,
    'h' ,
    'j' ,
    'k' ,
    'l' ,
    ';' ,
    '\'',
    '`' ,
    0   , // Left shift
    '\\',
    'z' ,
    'x' ,
    'c' ,
    'v' ,
    'b' ,
    'n' ,
    'm' ,
    ',' ,
    '.' ,
    '/' ,
    0   , // Right shift
    '*' ,
    0   , // Alt
    ' ' , // Space bar
    0   , // Caps lock
    0   , // F1
    0   , // F2
    0   , // F3
    0   , // F4
    0   , // F5
    0   , // F6
    0   , // F7
    0   , // F8
    0   , // F9
    0   , // F10
    0   , // Num lock
    0   , // Scroll Lock
    0   , // Home key
    0   , // Up Arrow
    0   , // Page Up
    '-' ,
    0   , // Left Arrow
    0   ,
    0   , // Right Arrow
    '+' ,
    0   , // End key
    0   , // Down Arrow
    0   , // Page Down
    0   , // Insert Key
    0   , // Delete Key
    0   ,
    0   ,
    0   ,
    0   , // F11
    0   , // F12
    0   , // All other keys are undefined
};

const char Keyboard::SHIFTED_ASCII_MAP[Keyboard::SCAN_CODES] = {
    0   , // NUL
    27  , // ESC
    '!' ,
    '@' ,
    '#' ,
    '$' ,
    '%' ,
    '^' ,
    '&' ,
    '*' ,
    '(' ,
    ')' ,
    '_' ,
    '+' ,
    '\b', // Backspace
    '\t', // Tab
    'Q' ,
    'W' ,
    'E' ,
    'R' ,
    'T' ,
    'Y' ,
    'U' ,
    'I' ,
    'O' ,
    'P' ,
    '{' ,
    '}' ,
    '\n', // Enter key
    0   , // Ctrl
    'A' ,
    'S' ,
    'D' ,
    'F' ,
    'G' ,
    'H' ,
    'J' ,
    'K' ,
    'L' ,
    ':' ,
    '"' ,
    '~' ,
    0   , // Left shift
    '|',
    'Z' ,
    'X' ,
    'C' ,
    'V' ,
    'B' ,
    'N' ,
    'M' ,
    '<' ,
    '>' ,
    '?' ,
    0   , // Right shift
    '*' ,
    0   , // Alt
    ' ' , // Space bar
    0   , // Caps lock
    0   , // F1
    0   , // F2
    0   , // F3
    0   , // F4
    0   , // F5
    0   , // F6
    0   , // F7
    0   , // F8
    0   , // F9
    0   , // F10
    0   , // Num lock
    0   , // Scroll Lock
    0   , // Home key
    0   , // Up Arrow
    0   , // Page Up
    '-' ,
    0   , // Left Arrow
    0   ,
    0   , // Right Arrow
    '+' ,
    0   , // End key
    0   , // Down Arrow
    0   , // Page Down
    0   , // Insert Key
    0   , // Delete Key
    0   ,
    0   ,
    0   ,
    0   , // F11
    0   , // F12
    0   , // All other keys are undefined
};

void Keyboard::install() {
    IRQ::handle(1, Keyboard::handler);
}

void Keyboard::handler(Context::Registers&) {
    if (!Scheduler::started())
        return;

    auto scan = x86::inb(SCAN_PORT);

    if (scan & SCAN_CODE_RELEASED) {
        scan &= ~SCAN_CODE_RELEASED;

        if (scan == SCAN_CODE_LEFT_SHIFT || scan == SCAN_CODE_RIGHT_SHIFT)
            shifted = false;

        return;
    }

    if (scan == SCAN_CODE_LEFT_SHIFT || scan == SCAN_CODE_RIGHT_SHIFT) {
        shifted = true;
        return;
    }

    if (auto function = getFunction(scan)) {
        Terminal::setActive(function - 1);
        return;
    }

    char key = shifted ? SHIFTED_ASCII_MAP[scan] : ASCII_MAP[scan];
    if (key) {
        Terminal::getActive().sendInput(key);
    }
}

int Keyboard::getFunction(int scan) {
    switch (scan) {
        case SCAN_CODE_F1:  return  1;
        case SCAN_CODE_F2:  return  2;
        case SCAN_CODE_F3:  return  3;
        case SCAN_CODE_F4:  return  4;
        case SCAN_CODE_F5:  return  5;
        case SCAN_CODE_F6:  return  6;
        case SCAN_CODE_F7:  return  7;
        case SCAN_CODE_F8:  return  8;
        case SCAN_CODE_F9:  return  9;
        case SCAN_CODE_F10: return 10;
        case SCAN_CODE_F11: return 11;
        case SCAN_CODE_F12: return 12;
        default:            return  0;
    }
}

