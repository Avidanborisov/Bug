#ifndef DATETIME_HPP
#define DATETIME_HPP

#include <stdint.h>

struct DateTime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t unused;

    bool operator==(const DateTime& rhs) const {
        return year    == rhs.year &&
               month   == rhs.month &&
               day     == rhs.day &&
               hour    == rhs.hour &&
               minutes == rhs.minutes &&
               seconds == rhs.seconds;
    }

    bool operator!=(const DateTime& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const DateTime& rhs) const {
        if (year != rhs.year)
            return year < rhs.year;

        if (month != rhs.month)
            return month < rhs.month;

        if (day != rhs.day)
            return day < rhs.day;

        if (hour != rhs.hour)
            return hour < rhs.hour;

        if (minutes != rhs.minutes)
            return minutes < rhs.minutes;

        return seconds < rhs.seconds;
    }

    bool operator<=(const DateTime& rhs) const {
        return !(*this > rhs);
    }

    bool operator> (const DateTime& rhs) const {
        return rhs < *this;
    }

    bool operator>=(const DateTime& rhs) const {
        return !(*this < rhs);
    }
};

#endif // DATETIME_HPP
