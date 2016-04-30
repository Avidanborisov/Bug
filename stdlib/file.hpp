#ifndef FILE_HPP
#define FILE_HPP

#include "string.hpp"

class File {
public:
    static String read(const char* file);
    static bool write(const char* file, const String& str);

    static String read(const String& file);
    static bool write(const String& file, const String& str);
};

#endif // FILE_HPP
