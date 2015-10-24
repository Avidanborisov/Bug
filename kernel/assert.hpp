#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "kernel.hpp"

#define assert(cond) \
    do { \
        if (!(cond)) { \
            Kernel::panic("%s:%u in %s: condition (" #cond ") failed\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        } \
    } while (false)

#endif // ASSERT_HPP
