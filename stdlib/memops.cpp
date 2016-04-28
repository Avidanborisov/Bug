#include <stdint.h>
#include "memops.hpp"

extern "C" {

int memcmp(const void* ap, const void* bp, size_t size) {
    auto a = static_cast<const uint8_t*>(ap);
    auto b = static_cast<const uint8_t*>(bp);

    size_t i;
    for (i = 0; i < size; ++i) {
        if (a[i] < b[i])
            return -1;
        if (a[i] > b[i])
            return 1;
    }

    return 0;
}

void* memset(void* bufp, int value, size_t size) {
    auto buf = static_cast<uint8_t*>(bufp);

    size_t i;
    for (i = 0; i < size; ++i)
        buf[i] = (uint8_t)value;

    return bufp;
}

void* memcpy(void* dstp, const void* srcp, size_t size) {
    auto dst = static_cast<uint8_t*>(dstp);
    auto src = static_cast<const uint8_t*>(srcp);

    size_t i;
    for (i = 0; i < size; ++i)
        dst[i] = src[i];

    return dstp;
}

void* memmove(void* dstp, const void* srcp, size_t size) {
    auto dst = static_cast<uint8_t*>(dstp);
    auto src = static_cast<const uint8_t*>(srcp);

    if (dstp == srcp || size == 0)
        return dstp;

    if (dstp < srcp)
        return memcpy(dstp, srcp, size);

    size_t i;
    for (i = size; i != 0; --i)
        dst[i - 1] = src[i - 1];

    return dstp;
}

}
