#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "task.h"
#include "paging.hpp"
#include "array.hpp"
#include "vector.hpp"

class Scheduler {
public:
    static constexpr auto FREQUENCY    = 1000; // ticks per second, (tick every ms)
    static constexpr auto QUANTUM      = 10;
    static constexpr auto TURNOVER     = 4;

    static constexpr auto STACK_PAGES        = 2;
    static constexpr auto STACK_SIZE         = STACK_PAGES * Paging::PAGE_SIZE;
    static constexpr auto VIRTUAL_USER_STACK = Paging::VIRTUAL_USERSPACE_END - STACK_SIZE;

    static constexpr auto CHILD_ANY = -1;
    static constexpr auto CHILD_ALL = -2;

    using Entry = void();

    static void start(Entry* entry);
    static void tick();
    static bool started();

    static int exec(const char* executable, int tty = -1);
    static int current();

    static void block(int pid);
    static void block(); // block current

    static void kill(int pid);
    static void exit(); // kill current

    static int wait(int pid, int child); // make pid wait for his child
    static int wait(int child); // make current wait for child

    static void sleep(int pid, size_t ms);
    static void sleep(size_t ms); // make current sleep

    static void unblock(int pid);

    static uint32_t increase(int pid, size_t pages);
    static uint32_t increase(size_t pages);

    static Vector<String> getWorkingDirectory(int pid);
    static Vector<String> getWorkingDirectory();

    static void setWorkingDirectory(int pid, const Vector<String>& path);
    static void setWorkingDirectory(const Vector<String>& path);

    static size_t tty(int pid);

    static bool isKernelTask(int pid);

private:
    static bool enabled;

    static Vector<Task> tasks;
    static int currPid;

    static void reschedule();
    static int nextTask();
    static void switchTo(int newPid);
    static bool shouldWait(int pid, bool childJustKilled);

    static struct KernelTask {
        uint8_t kernelStack[STACK_SIZE];
        uint8_t userStack[STACK_SIZE];
        int pid;

        static void idle();
        static void cleaner();
    } idle, cleaner, init;

    static int newTask();
    static int newKernelTask(const char* name, uint8_t* kernelStack, uint8_t* userStack, Entry* entry);
    static int newUserTask(const char* executable);

    static uint32_t espAddress(uint32_t stackStart, uint32_t offset, size_t size = STACK_SIZE);
    static uint32_t espAddress(uint8_t* stackStart, uint32_t offset, size_t size = STACK_SIZE);

    static uint32_t getTaskCr3(int pid)                  asm("Scheduler_getTaskCr3");
    static Context::Registers** getTaskContext(int pid)  asm("Scheduler_getTaskContext");
};

#endif // SCHEDULER_HPP
