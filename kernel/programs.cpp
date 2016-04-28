#include "programs.hpp"
#include "string.hpp"

extern uint8_t _binary_shell_start[];
extern uint8_t _binary_shell_end[];
extern uint8_t _binary_shell_size[];

Programs::Object Programs::objects[] = {
    { "shell", _binary_shell_start, _binary_shell_end, reinterpret_cast<size_t>(_binary_shell_size) }
};

const Programs::Object* Programs::get(const char* program) {
    for (const auto& o : objects) {
        if (String::compare(o.name, program) == 0)
            return &o;
    }

    return nullptr;
}
