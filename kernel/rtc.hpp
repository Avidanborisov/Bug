#ifndef RTC_HPP
#define RTC_HPP

#include "datetime.hpp"

class RTC {
public:
    static DateTime get();

private:
    static constexpr auto CMOS_ADDRESS_PORT = 0x70;
    static constexpr auto CMOS_DATA_PORT    = 0x71;

    static void waitToUpdate();
    static uint8_t getRegister(uint8_t reg);
    static DateTime readRegisters();
};

#endif // RTC_HPP
