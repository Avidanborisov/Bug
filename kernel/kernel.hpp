#ifndef KERNEL_HPP
#define KERNEL_HPP

class Kernel {
public:
    static void main() asm("kmain");
    static int test asm("kernel_test");
};

#endif // KERNEL_HPP
