#include "kernel.hpp"

void Kernel::main() {
    *(unsigned char *)0xB8CD0 = 'A'; // print 'A' on the screen somewhere visible
}
