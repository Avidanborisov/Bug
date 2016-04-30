#ifndef WAITQUEUE_HPP
#define WAITQUEUE_HPP

#include "lockguard.hpp"
#include "spinlock.hpp"
#include "scheduler.hpp"
#include "queue.hpp"

class WaitQueue {
public:
    void wait() {
        lock.acquire();

        auto curr = Scheduler::current();
        tasks.push(curr);

        lock.release();
        Scheduler::block(curr);
    }

    void wake() {
        LockGuard<Spinlock> guard(lock);

        if (tasks.empty())
            return;

        auto taskToWake = tasks.top();
        tasks.pop();

        Scheduler::unblock(taskToWake);
    }

private:
    Spinlock lock;
    Queue<int> tasks;
};

#endif // WAITQUEUE_HPP
