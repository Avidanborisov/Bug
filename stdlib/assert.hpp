#ifndef ASSERT_HPP
#define ASSERT_HPP

extern "C" void abort(const char* msg, ...);

#define assert(cond) \
    do { \
        if (!(cond)) { \
            abort("%s:%u in %s: condition (" #cond ") failed\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        } \
    } while (false)

#endif // ASSERT_HPP
