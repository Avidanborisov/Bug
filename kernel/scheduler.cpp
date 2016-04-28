#include "kernel.hpp"
#include "scheduler.hpp"
#include "paging.hpp"
#include "x86.hpp"
#include "context.hpp"
#include "console.hpp"
#include "timer.hpp"
#include "tss.hpp"
#include "physicalallocator.hpp"
#include "programs.hpp"
#include "elf.hpp"
#include "vector.hpp"
#include "virtualallocator.hpp"
#include "mappedpointer.hpp"
#include "utility.hpp"

Vector<Task> Scheduler::tasks;
int Scheduler::currPid = -1;
bool Scheduler::started = false;

Scheduler::KernelTask Scheduler::idle;
Scheduler::KernelTask Scheduler::cleaner;
Scheduler::KernelTask Scheduler::init;

uint32_t Scheduler::getTaskCr3(int pid) {
    return tasks[pid].cr3;
}

Context::Registers** Scheduler::getTaskContext(int pid) {
    return &tasks[pid].context;
}

uint32_t Scheduler::espAddress(uint32_t stackStart, uint32_t offset, size_t size) {
    auto top = stackStart + size - offset;
    return Math::roundDown(top, 4ul) - 4;
}

uint32_t Scheduler::espAddress(uint8_t* stackStart, uint32_t offset, size_t size) {
    return espAddress(reinterpret_cast<uint32_t>(stackStart), offset, size);
}

int Scheduler::newTask() {
    size_t pid = 0;
    for (; pid < tasks.size(); ++pid) {
        if (tasks[pid].state == Task::State::EMPTY)
            break;
    }

    if (pid == tasks.size())
        tasks.add();

    tasks[pid].pid = pid;
    tasks[pid].parent = currPid;
    tasks[pid].state = Task::State::NEW;
    tasks[pid].rounds = 0;

    return pid;
}

int Scheduler::newKernelTask(uint8_t* kernelStack, uint8_t* userStack, Entry* entry) {
    auto& task = tasks[newTask()];
    auto esp = espAddress(userStack, sizeof(Context::Registers));

    task.context = reinterpret_cast<Context::Registers*>(esp);
    task.context->cs = GDT::CODE_SELECTOR;
    task.context->ds = GDT::DATA_SELECTOR;
    task.context->eflags = x86::Flags::IF; // interrupts enabled
    task.context->eip = reinterpret_cast<uint32_t>(entry);
    task.context->esp = esp;
    task.espKernel = espAddress(kernelStack, 0);
    task.cr3 = x86::regs::cr3();

    task.state = Task::State::READY;
    return task.pid;
}

int Scheduler::newUserTask(const char* executable) {
    const Programs::Object* program = Programs::get(executable);
    if (!program)
        return -1;

    auto& task = tasks[newTask()];
    Paging::mapKernel(task);

    auto userStackSegments = VirtualAllocator::link(task, VIRTUAL_USER_STACK, STACK_PAGES, Paging::Flags::WRITE);
    auto last = userStackSegments.last();

    MappedPointer<uint8_t> userStack(last.physical, last.pages);
    auto espAccessible = espAddress(userStack.get(), sizeof(Context::Registers), last.pages * Paging::PAGE_SIZE);
    auto espVirtual    = espAddress(VIRTUAL_USER_STACK, sizeof(Context::Registers));

    auto header = reinterpret_cast<ELF::Header*>(program->start);

    auto context = reinterpret_cast<Context::Registers*>(espAccessible);
    context->cs = GDT::USER_CODE_SELECTOR;
    context->ds = GDT::USER_DATA_SELECTOR;
    context->ss = GDT::USER_DATA_SELECTOR;
    context->eflags = x86::Flags::IF; // interrupts enabled
    context->eip = header->entry();
    context->esp = espVirtual;
    task.context = reinterpret_cast<Context::Registers*>(espVirtual);
    task.end = 0;

    Vector<uint8_t> emptyContent;

    for (size_t i = 0; i < header->programHeaders(); ++i) {
        auto& programHeader = header->programHeaderTable()[i];

        // if loadable segment
        if (programHeader.isLoadable()) {
            uint32_t start = programHeader.virtualAddress();
            uint32_t size  = programHeader.memorySize();

            auto virt    = Paging::alignDown(start);
            auto bytes   = Math::roundUp(size, Paging::PAGE_SIZE);
            auto pages   = bytes / Paging::PAGE_SIZE;
            auto padding = virt - start;

            if (virt + bytes > task.end)
                task.end = virt + bytes;

            auto added = VirtualAllocator::link(task, virt, pages, programHeader.flags());

            auto content = programHeader.data(header);
            if (programHeader.fileSize() != programHeader.memorySize()) { // if bss, create zero'd content
                emptyContent.clear();
                emptyContent.resize(programHeader.memorySize() - programHeader.fileSize(), 0);
                emptyContent.insert(0, content, content + programHeader.fileSize());
                content = emptyContent.begin();
            }

            for (auto i = 0; size > 0; ++i) {
                const auto& segment = added[i];

                MappedPointer<uint8_t> mem(segment.physical, segment.pages);
                auto bytesToCopy = Util::min(segment.pages * Paging::PAGE_SIZE - padding, size);
                Util::copy(mem.get() + padding, content, content + bytesToCopy);

                size -= bytesToCopy;
                content += bytesToCopy;
                padding = 0;
            }
        }
    }

    task.espKernel = espAddress(VirtualAllocator::linked(task, STACK_PAGES), 0);

    task.state = Task::State::READY;
    return task.pid;
}

void Scheduler::start(Entry* entry) {
    idle.pid    = newKernelTask(idle.kernelStack, idle.userStack, KernelTask::idle);
    cleaner.pid = newKernelTask(cleaner.kernelStack, cleaner.userStack, KernelTask::cleaner);
    init.pid    = newKernelTask(init.kernelStack, init.userStack, entry);

    currPid = idle.pid;
    tasks[currPid].state = Task::State::RUNNING;
    Task::init(currPid);
}

void Scheduler::tick() {
    if (!started)
        return;

    static auto ticks = 0;
    ++ticks;

    for (auto& task : tasks) {
        if (task.state == Task::State::SLEEPING && --task.timeout == 0) {
            task.state = Task::State::READY;
        }
    }

    if (ticks % QUANTUM != 0)
        return;

    auto& currTask = tasks[currPid];
    if (currTask.rounds != TURNOVER) {
        ++currTask.rounds;
        return;
    }

    currTask.rounds = 0;
    currTask.state = Task::State::READY;
    reschedule();
}

void Scheduler::reschedule() {
    if (tasks[currPid].state != Task::State::RUNNING) {
        int newPid = nextTask();

        if (newPid == currPid) {
            tasks[currPid].state = Task::State::RUNNING;
            return;
        }

        switchTo(newPid);
    }
}

int Scheduler::nextTask() {
    for (size_t i = 1; i < tasks.size(); ++i) {
        int pid = (i + currPid) % tasks.size();
        auto& task = tasks[pid];

        if (pid != idle.pid && task.state == Task::State::READY)
            return task.pid;
    }

    return idle.pid;
}

void Scheduler::switchTo(int newPid) {
    int oldPid = currPid;
    currPid = newPid;

    tasks[currPid].state = Task::State::RUNNING;
    TSS::espKernel() = tasks[currPid].espKernel;
    Task::replace(oldPid, currPid);
}

int Scheduler::exec(const char* executable) {
    return newUserTask(executable);
}

int Scheduler::current() {
    return currPid;
}

void Scheduler::set(int pid, Task::State state) {
    assert(pid > 0 && size_t(pid) < tasks.size());
    tasks[pid].state = state;
}

void Scheduler::block(int pid) {
    assert(pid > 0 && size_t(pid) < tasks.size());
    tasks[pid].state = Task::State::BLOCKED;
    reschedule();
}

void Scheduler::sleep(int pid, size_t ms) {
    assert(pid > 0 && size_t(pid) < tasks.size());

    tasks[pid].state = Task::State::SLEEPING;
    tasks[pid].timeout = ms;

    reschedule();
}

bool Scheduler::shouldWait(int pid, bool childJustKilled) {
    const Task& task = tasks[pid];

    if (task.waitingFor > 0)
        return tasks[task.waitingFor].state != Task::State::KILLED;

    if (task.waitingFor == CHILD_ANY && childJustKilled)
        return false;

    if (task.waitingFor == CHILD_ALL) {
        for (const Task& child : tasks) {
            if (child.parent == pid && task.state != Task::State::KILLED)
                return true;
        }

        return false;
    }

    Kernel::panic("Invalid pid waiting for");
}

void Scheduler::kill(int pid) {
    assert(pid > 0 && size_t(pid) < tasks.size());
    auto parent = tasks[pid].parent;

    tasks[pid].state = Task::State::KILLED;

    // unblock parent
    for (const Task& task : tasks) {
        if (task.pid == parent && task.state == Task::State::WAITING) {
            if (!shouldWait(task.pid, true))
                unblock(task.pid);
        }
    }

    unblock(cleaner.pid);
    reschedule();
}

void Scheduler::wait(int pid, int child) {
    assert(pid > 0 && size_t(pid) < tasks.size());

    tasks[pid].waitingFor = child;

    if (!shouldWait(pid, false))
        return;

    tasks[pid].state = Task::State::WAITING;
    reschedule();
}

void Scheduler::unblock(int pid) {
    assert(pid > 0 && size_t(pid) < tasks.size());

    if (tasks[pid].state != Task::State::BLOCKED && tasks[pid].state != Task::State::WAITING)
        return;

    tasks[pid].state = Task::State::READY;
}

uint32_t Scheduler::increase(int pid, size_t pages) {
    assert(pid > 0 && size_t(pid) < tasks.size());
    auto& task = tasks[pid];

    if (VirtualAllocator::link(task, task.end, pages, Paging::Flags::WRITE).size() > 0) {
        auto addr = task.end;
        task.end += pages * Paging::PAGE_SIZE;
        return addr;
    }

    return task.end;
}

void Scheduler::set(Task::State state) {
    set(currPid, state);
}

void Scheduler::block() {
    block(currPid);
}

void Scheduler::sleep(size_t ms) {
    sleep(currPid, ms);
}

void Scheduler::exit() {
    kill(currPid);
}

void Scheduler::wait(int child) {
    wait(currPid, child);
}

uint32_t Scheduler::increase(size_t pages) {
    return increase(currPid, pages);
}

void Scheduler::KernelTask::idle() {
    started = true;
    Timer::start(FREQUENCY);

    while (true) {
        x86::hlt();
    }
}

void Scheduler::KernelTask::cleaner() {
    while (true) {
        block(); // wait for something to be done

        for (Task& task : tasks) {
            if (task.state != Task::State::KILLED)
                continue;

            for (const auto& segment : task.segments) {
                if (segment.virt != 0)
                    Paging::unmap(task, segment.virt, segment.pages);
                PhysicalAllocator::free(segment.physical, segment.pages);
            }

            PhysicalAllocator::free(task.cr3, 1);

            task.state = Task::State::EMPTY;
            task.segments.clear();
        }
    }
}
