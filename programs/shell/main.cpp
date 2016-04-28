#include <console.hpp>
#include <system.hpp>

void main() {
    for (int i = 0; i < 10; ++i) {
        Console::print("hello\n");
        System::sleep(2);
    }
}
