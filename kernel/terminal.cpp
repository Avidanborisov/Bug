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
        monitor()[cursor.y][cursor.x] = Framebuffer::BLANK;
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

void Terminal::sendInput(char key) {
    int entered = 0;

    for (char c : input) {
        if (c == '\b')
            --entered;
        else
            ++entered;
    }

    if (key == '\b') {
        if (entered > 0) {
            if (!dp) {
                Console::print(key);
            }
            input.insert(0, { key });
        }

        return;
    }

    input.insert(0, { key });
    if (!dp) {
        Console::print(key);
    }
    inputWaiters.wake();
}

bool Terminal::hasInput() {
    return input.size() > 0;
}

size_t Terminal::read(char* buffer, size_t max) {
    while (true) {
        int entered = 0;
        char last = 0;

        if (!input.empty()) {
            last = input.first();
            for (char c : input) {
                if (c == '\b')
                    --entered;
                else
                    ++entered;
            }
        }

        if (entered > 0 && (last == '\n' || size_t(entered) == max)) {
            size_t pos = 0;

            while (!input.empty()) {
                char c = input.last();
                input.pop();

                if (c == '\b') {
                    if (pos > 0)
                        --pos;
                } else {
                    buffer[pos++] = c;
                }
            }

            return pos;
        }

        inputWaiters.wait();
    }
}

void Terminal::setPosition(int x, int y) {
    cursor = { (uint8_t)x, (uint8_t)y };
    updateCursor();
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

void Terminal::setActive(size_t tty) {
    if (active == tty)
        return;

    Terminal& currActive = terminals[active];
    Terminal& newActive  = terminals[tty];

    memcpy(currActive.virtualMonitor(), physicalMonitor, sizeof(Framebuffer::Monitor));
    memcpy(physicalMonitor, newActive.virtualMonitor(), sizeof(Framebuffer::Monitor));

    active = tty;
    newActive.updateCursor();
}

size_t Terminal::getActiveTTY() {
    return active;
}

Terminal& Terminal::getActive() {
    return terminals[active];
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

void Terminal::dontPrint(bool status) {
    dp = status;
}

void clear() {
    Terminal::getActive().clear();
}

void putchar(char c, Console::Color fg, Console::Color bg) {
    Terminal::getActive().putchar(c, fg, bg);
}

void print(const char* s, Console::Color fg, Console::Color bg) {
    Terminal::getActive().print(s, fg, bg);
}

size_t input(char* buffer, size_t max) {
    return Terminal::getActive().read(buffer, max);
}

bool hasInput() {
    return Terminal::getActive().hasInput();
}

void setPosition(int x, int y) {
    return Terminal::getActive().setPosition(x, y);
}

void dontPrint(bool status) {
    return Terminal::getActive().dontPrint(status);
}
