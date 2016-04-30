#ifndef SYSFS_HPP
#define SYSFS_HPP

#include "filesystem.hpp"
#include "vector.hpp"
#include "string.hpp"

class Sysfs : public Filesystem::Backend {
public:
    using ValueFunction = String(*)(void);

    ~Sysfs() = default;
    Sysfs(const String& mp);

    virtual int read(const Vector<String>& pathParts, char* buffer, size_t count, size_t offset) override;
    virtual int write(const Vector<String>& pathParts, const char* buffer, size_t count, size_t offset) override;

    static void set(const String& path, const String& value, bool readonly = true);
    static void set(const String& path, ValueFunction function, bool readonly = true);

    static void remove(const String& path);

    static String fileList();
    static String directoryList();

private:
    struct Value {
        Value(String name, String value, bool readonly = true);
        Value(String name, ValueFunction func, bool readonly = true);

        String get();

        String name;
        String value;
        ValueFunction function = nullptr;
        bool readonly;
    };

    struct Directory {
        String name;
        Vector<Directory> subdirs;
        Vector<Value> values;
    };

    static Vector<String> mountPoint;
    static Directory root;
};

#endif // SYSFS_HPP
