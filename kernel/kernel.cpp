#include <stdint.h>
#include "kernel.hpp"

void Kernel::main() {
    using Monitor = uint16_t[25][80];
    auto& monitor = *(Monitor *)0xB8000;

    for (auto& row : monitor)
        for (auto& cell : row)
            cell = ' ';

    monitor[0][0] = 'A' | 0xf00;
}
