#ifndef MEMOPS_HPP
#define MEMOPS_HPP

#include <stddef.h>

extern "C" {
    int memcmp(const void* ap, const void* bp, size_t size);
    void* memset(void* bufp, int value, size_t size);
    void* memcpy(void* dstp, const void* srcp, size_t size);
    void* memmove(void* dstp, const void* srcp, size_t size);
}

#endif // MEMOPS_HPP
