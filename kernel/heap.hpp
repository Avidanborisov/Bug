#ifndef HEAP_HPP
#define HEAP_HPP

#include <stddef.h>

class Heap {
public:
    static void init();

    static void* allocate(size_t size);
    static void free(void* ptr);

private:
    static constexpr size_t ALIGNMENT = 16;

    union Header {
        struct {
            Header* next;
            size_t size;
        };

        size_t units() const;

        void* data();
        static Header* meta(void* data);

        char padding[ALIGNMENT];
    };

    static_assert(sizeof(Header) == ALIGNMENT, "sizeof(Header) should be ALIGNMENT");

    static Header base;
    static Header* head;

    static Header* expand(size_t bytes);
};

#endif // HEAP_HPP
