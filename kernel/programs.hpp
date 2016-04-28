#ifndef PROGRAMS_HPP
#define PROGRAMS_HPP

#include <stdint.h>
#include <stddef.h>

class Programs {
public:
    struct Object {
        const char* name;
        uint8_t* start;
        uint8_t* end;
        size_t size;
    };

    static const Object* get(const char* program);

private:
    static Object objects[];
};

#endif // PROGRAMS_HPP
