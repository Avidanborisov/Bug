#include "file.hpp"

extern int read(const char* file, char* buffer, size_t count, size_t offset = 0);
extern int write(const char* file, const char* buffer, size_t count, size_t offset = 0);

String File::read(const char* file) {
    char buf[1024];
    int n = ::read(file, buf, sizeof buf);

    if (n <= 0)
        return "";

    return String(buf, n);
}

bool File::write(const char* file, const String& str) {
    return ::write(file, str.data(), str.length()) >= 0;
}

String File::read(const String& file) {
    return read(file.data());
}

bool File::write(const String& file, const String& str) {
    return write(file.data(), str);
}

