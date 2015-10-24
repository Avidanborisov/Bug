#ifndef KERNEL_HPP
#define KERNEL_HPP

class Kernel {
public:
    static void main() asm("kmain");
    static void panic(const char* msg, ...);
};

#endif // KERNEL_HPP
