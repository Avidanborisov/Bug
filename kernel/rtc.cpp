#include "rtc.hpp"
#include "x86.hpp"

void RTC::waitToUpdate() {
    do {
        x86::out(CMOS_ADDRESS_PORT, 0x0a);
    } while (x86::inb(CMOS_DATA_PORT) & 0x80);
}

uint8_t RTC::getRegister(uint8_t reg) {
    x86::out(CMOS_ADDRESS_PORT, reg);
    return x86::inb(CMOS_DATA_PORT);
}

DateTime RTC::readRegisters() {
    DateTime date;

    date.seconds = getRegister(0x0);
    date.minutes = getRegister(0x2);
    date.hour    = getRegister(0x4);
    date.day     = getRegister(0x7);
    date.month   = getRegister(0x8);
    date.year    = getRegister(0x9);

    return date;
}

DateTime RTC::get() {
    DateTime last, date = readRegisters();

    do {
        last = date;
        waitToUpdate();
        date = readRegisters();
    } while (last != date);

    auto regB = getRegister(0x0b);

    // convert BCD to binary if needed
    if (!(regB & 0x04)) {
        date.seconds = (date.seconds & 0x0F) + ((date.seconds / 16) * 10);
        date.minutes = (date.minutes & 0x0F) + ((date.minutes / 16) * 10);
        date.hour = ((date.hour & 0x0F) + (((date.hour & 0x70) / 16) * 10)) | (date.hour & 0x80);
        date.day = (date.day & 0x0F) + ((date.day / 16) * 10);
        date.month = (date.month & 0x0F) + ((date.month / 16) * 10);
        date.year = (date.year & 0x0F) + ((date.year / 16) * 10);
    }

    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(regB & 0x02) && (date.hour & 0x80)) {
        date.hour = ((date.hour & 0x7F) + 12) % 24;
    }

    // convert to current century
    const auto CURRENT_YEAR = 2016;
    date.year += CURRENT_YEAR / 100 * 100;
    if (date.year < CURRENT_YEAR) {
        date.year += 100;
    }

    return date;
}
