#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "interrupts.hpp"
#include "timer.hpp"
#include "memorymap.hpp"
#include "x86.hpp"
#include "physicalallocator.hpp"
#include "virtualallocator.hpp"
#include "paging.hpp"
#include "string.hpp"
#include "utility.hpp"
#include "heap.hpp"
#include "containers/uniquepointer.hpp"
#include "containers/vector.hpp"

struct Test {
    String value;

    Test() {
        Console::print("Test()\n");
    }

    Test(const char* value) : Test(String(value)) { }

    Test(const String& value) : value(value) {
        Console::printf("Test(%s)\n", value);
    }

    Test(Test const& other) noexcept : value(other.value) {
        Console::printf("Test(Test(%s))\n", value);
    }

    Test& operator=(const Test& other) {
        value = other.value;
        Console::printf("Test = Test(%s))\n", value);
        return *this;
    }

    Test& operator=(Test&& other) {
        value = Util::move(other.value);
        Console::printf("Test = move(Test(%s))\n", value);
        return *this;
    }

    Test(Test&& other) noexcept : value(Util::move(other.value)) {
        Console::printf("Test(move(Test(%s)))\n", value);
    }

    ~Test() {
        if (!value.empty())
            Console::printf("~Test(%s)\n", value);
    }
};

void Kernel::main() {
    Timer::disable();

    Console::init();
    GDT::init();
    Interrupts::init();

    MemoryMap::init();
    PhysicalAllocator::init();
    Paging::init();

    Vector<Test> d { "Hello", "Man" };
    d.resize(4);
    Vector<Test> f = d;
    f = { "asdf", "dig", "one", "two", "three", "four" };
    f.insert(1, {"asdfasdf", "ASdgaerg", "Asd"});
    f.insert(8, {"kklo", "ds"});

    for (const auto& x : f)
        Console::printf("%s ", x.value);
    Console::print('\n');

    Vector<UniquePointer<Test>> p;
    p.add(new Test("dyn"));
    p.reserve(10);
    p.resize(3);
}

void Kernel::panic(const char* msg, ...) {
    va_list ap;
    va_start(ap, msg);

    Console::print("\n*** KERNEL PANIC ***\n", Console::Color::RED);
    Console::vprintf(Console::Color::LIGHT_GREY, msg, ap);
    Console::print('\n');

    x86::cli();
    while (true) { }
}
