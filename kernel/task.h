#ifndef TASK_H
#define TASK_H

#include "context.hpp"
#include "vector.hpp"

class Task {
public:
    int pid;
    int parent;
    int waitingFor;

    size_t timeout;
    size_t rounds;

    enum class State {
        EMPTY,
        NEW,
        READY,
        RUNNING,
        BLOCKED,
        SLEEPING,
        WAITING,
        KILLED
    } state = State::EMPTY;

    uint32_t cr3;
    uint32_t espKernel;
    Context::Registers* context;

    struct Segment {
        uint32_t physical;
        uint32_t virt;
        size_t pages;
    };

    Vector<Segment> segments;
    uint32_t end;

private:
    static void init(int firstPid)               asm("Task_init");
    static void replace(int currPid, int newPid) asm("Task_replace");

    friend class Scheduler;
};

#endif // TASK_H
