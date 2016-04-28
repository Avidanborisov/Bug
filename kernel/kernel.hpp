#ifndef KERNEL_HPP
#define KERNEL_HPP

class Kernel {
public:
    static void main()                                   asm("kmain");
    [[noreturn]] static void panic(const char* msg, ...) asm("abort");
};

#endif // KERNEL_HPP
