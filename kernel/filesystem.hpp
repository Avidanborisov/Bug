#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "string.hpp"
#include "vector.hpp"

class Filesystem {
public:
    static void init();

    static int read(const char* file, char* buffer, size_t count, size_t offset = 0);
    static int write(const char* file, const char* buffer, size_t count, size_t offset = 0);

    class Backend {
    public:
        Backend(const String& mp);
        virtual ~Backend() = default;

        virtual int read(const Vector<String>& path, char* buffer, size_t count, size_t offset) = 0;
        virtual int write(const Vector<String>& path, const char* buffer, size_t count, size_t offset) = 0;

        Vector<String> mountPoint;
    };

private:
    static Vector<String> getPath(const char* filePath);
    static Filesystem::Backend& getBackend(const Vector<String>& path);
    static Vector<String> getBackendPath(const Vector<String>& path, const Backend& fs);

    static Vector<Backend*> mounted;
};

#endif // FILESYSTEM_HPP
