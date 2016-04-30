#ifndef LOCKGUARD_HPP
#define LOCKGUARD_HPP

template<class Lock>
class LockGuard {
public:
    explicit LockGuard(Lock& lock) : lock(lock) {
        lock.acquire();
    }

    ~LockGuard() {
        lock.release();
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard(LockGuard&&) = delete;

    LockGuard& operator=(const LockGuard&) = delete;
    LockGuard& operator=(LockGuard&&) = delete;

private:
    Lock& lock;
};

#endif // LOCKGUARD_HPP
