#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "spinlock.hpp"
#include "vector.hpp"
#include "scheduler.hpp"

class Semaphore {
public:
    Semaphore(size_t value) : value(value) { }

    void test() {
        lock.acquire();

        if (value == 0) {
            queue.add(Scheduler::current());
            lock.release();
            Scheduler::block();
            lock.acquire();
        }

        --value;
        lock.release();
    }

    void release() {
        LockGuard<Spinlock> guard(lock);

        ++value;
        if (!queue.empty()) {
            auto pid = queue.last();
            queue.pop();

            Scheduler::unblock(pid);
        }
    }

private:
    Spinlock lock;
    Vector<int> queue;
    volatile size_t value;
};

#endif // SEMAPHORE_HPP
