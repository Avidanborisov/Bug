#include "terminal.hpp"
#include "memops.hpp"

Framebuffer::Monitor& Terminal::physicalMonitor =
    *reinterpret_cast<Framebuffer::Monitor*>(Framebuffer::PHYSICAL_MEMORY_START);

size_t Terminal::active;
Terminal Terminal::terminals[Terminal::NUM_TERMINALS];

void Terminal::clear(Framebuffer::Monitor& monitor) {
    for (auto& row : monitor)
        for (auto& cell : row)
            cell = Framebuffer::BLANK;
}

void Terminal::clear() {
    clear(monitor());
    cursor = { 0, 0 };
    updateCursor();
}

void Terminal::putchar(char c, Console::Color fg, Console::Color bg) {
    if (c == '\b' && cursor.x != 0) {
        --cursor.x;
    } else if (c == '\t') {
        cursor.x = (cursor.x / 8 + 1) * 8;
    } else if (c == '\r') {
        cursor.x = 0;
    } else if (c == '\n') {
        cursor.x = 0;
        ++cursor.y;
    } else if (c >= ' ') { // printable
        monitor()[cursor.y][cursor.x] = Framebuffer::makeChar(c, fg, bg);
        ++cursor.x;
    }

    if (cursor.x >= Framebuffer::WIDTH) {
        cursor.x = 0;
        ++cursor.y;
    }

    scroll();
    updateCursor();
}

void Terminal::print(const char* s, Console::Color fg, Console::Color bg) {
    while (*s != '\0')
        putchar(*s++, fg, bg);
}

void Terminal::scroll() {
    if (cursor.y < Framebuffer::HEIGHT)
        return;

    // move all lines upwords
    for (auto i = 1; i < Framebuffer::HEIGHT; ++i)
        for (auto j = 0; j < Framebuffer::WIDTH; ++j)
            monitor()[i - 1][j] = monitor()[i][j];

    // clear last line
    for (auto i = 0; i < Framebuffer::WIDTH; ++i)
        monitor()[Framebuffer::HEIGHT - 1][i] = Framebuffer::BLANK;

    // set y offset to last line
    cursor.y = Framebuffer::HEIGHT - 1;
}

size_t Terminal::id() const {
    return this - terminals;
}

void Terminal::setActive(int tty) {
    Terminal& currActive = terminals[active];
    Terminal& newActive  = terminals[tty];

    memcpy(currActive.virtualMonitor(), physicalMonitor, sizeof(Framebuffer::Monitor));
    memcpy(physicalMonitor, newActive.virtualMonitor(), sizeof(Framebuffer::Monitor));

    active = tty;
    newActive.updateCursor();
}

int Terminal::getActive() {
    return active;
}

Terminal& Terminal::get(int tty) {
    return terminals[tty];
}

Framebuffer::Monitor& Terminal::virtualMonitor() {
    if (!backend) {
        backend = reinterpret_cast<Framebuffer::Monitor*>(new Framebuffer::Monitor);
        clear(*backend);
    }

    return *backend;
}

Framebuffer::Monitor& Terminal::monitor() {
    return id() == active ? physicalMonitor : virtualMonitor();
}

void Terminal::updateCursor() {
    if (id() == active)
        Framebuffer::Cursor::update(cursor.x, cursor.y);
}

void clear() {
    Terminal::get(Terminal::getActive()).clear();
}

void putchar(char c, Console::Color fg, Console::Color bg) {
    Terminal::get(Terminal::getActive()).putchar(c, fg, bg);
}

void print(const char* s, Console::Color fg, Console::Color bg) {
    Terminal::get(Terminal::getActive()).print(s, fg, bg);
}
