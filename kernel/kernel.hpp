#ifndef KERNEL_HPP
#define KERNEL_HPP

class Kernel {
public:
    static void main() asm("kmain");
};

#endif // KERNEL_HPP