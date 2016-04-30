#ifndef SPINLOCK_HPP
#define SPINLOCK_HPP

class Spinlock {
public:
    void acquire() {
        // atomically write 1 to lock and return previous value
        // if previous value was 0, the lock is acquire and the function returns
        // if previous value was 1, we wait until it's 0
        while (__sync_lock_test_and_set(&lock, 1) == 1) { }
    }

    void release() {
        __sync_synchronize(); // memory barrier
        lock = 0;
    }

private:
    volatile size_t lock = 0;
};

#endif // SPINLOCK_HPP
