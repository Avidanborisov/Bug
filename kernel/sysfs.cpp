#include "sysfs.hpp"
#include "memops.hpp"
#include "scheduler.hpp"

Sysfs::Directory Sysfs::root;
Vector<String> Sysfs::mountPoint;

Sysfs::Value::Value(String name, String value, bool readonly) :
    name(name), value(value), readonly(readonly) { }

Sysfs::Value::Value(String name, Sysfs::ValueFunction func, bool readonly) :
    name(name), function(func), readonly(readonly) { }

String Sysfs::Value::get() {
    return function ? function() : value;
}

Sysfs::Sysfs(const String& mp) : Filesystem::Backend(mp) {
    mountPoint = mp.split('/');
}

int Sysfs::read(const Vector<String>& pathParts, char* buffer, size_t count, size_t offset) {
    if (pathParts.size() == 0)
        return -1;

    Directory* directory = &root;
    for (size_t i = 0; i < pathParts.size() - 1; ++i) {
        auto dir = pathParts[i];

        bool found = false;
        for (Directory& subdir : directory->subdirs) {
            if (subdir.name == dir) {
                directory = &subdir;
                found = true;
                break;
            }
        }

        if (!found) {
            return -1;
        }
    }

    for (Value& val : directory->values) {
        if (val.name == pathParts.last()) {
            auto value = val.get();
            if (offset > value.length())
                return -1;

            auto read = Util::min(count, value.length() - offset);
            memcpy(buffer, value.data() + offset, read);
            return read;
        }
    }

    return -1;
}

int Sysfs::write(const Vector<String>& pathParts, const char* buffer, size_t count, size_t offset) {
    if (pathParts.size() == 0)
        return -1;

    Directory* directory = &root;
    for (size_t i = 0; i < pathParts.size() - 1; ++i) {
        auto dir = pathParts[i];

        bool found = false;
        for (Directory& subdir : directory->subdirs) {
            if (subdir.name == dir) {
                directory = &subdir;
                found = true;
                break;
            }
        }

        if (!found) {
            directory->subdirs.add();
            directory->subdirs.last().name = dir;
            directory = &directory->subdirs.last();
        }
    }

    for (Value& val : directory->values) {
        if (val.name == pathParts.last()) {
            if (!val.readonly) {
                val.value = String(buffer + offset, count);
                return count;
            }
        }
    }

    directory->values.add(pathParts.last(), String(buffer + offset, count), false);
    return count;
}

void Sysfs::set(const String& path, const String& value, bool readonly) {
    auto pathParts = path.split('/');
    if (pathParts.size() == 0)
        return;

    Directory* directory = &root;
    for (size_t i = 0; i < pathParts.size() - 1; ++i) {
        auto dir = pathParts[i];

        bool found = false;
        for (Directory& subdir : directory->subdirs) {
            if (subdir.name == dir) {
                directory = &subdir;
                found = true;
                break;
            }
        }

        if (!found) {
            directory->subdirs.add();
            directory->subdirs.last().name = dir;
            directory = &directory->subdirs.last();
        }
    }

    for (Value& val : directory->values) {
        if (val.name == pathParts.last()) {
            if (!val.readonly) {
                val.value = value;
                val.function = nullptr;
            }

            return;
        }
    }

    directory->values.add(pathParts.last(), value, readonly);
}

void Sysfs::set(const String& path, ValueFunction function, bool readonly) {
    auto pathParts = path.split('/');
    if (pathParts.size() == 0)
        return;

    Directory* directory = &root;
    for (size_t i = 0; i < pathParts.size() - 1; ++i) {
        auto dir = pathParts[i];

        bool found = false;
        for (Directory& subdir : directory->subdirs) {
            if (subdir.name == dir) {
                directory = &subdir;
                found = true;
                break;
            }
        }

        if (!found) {
            directory->subdirs.add();
            directory->subdirs.last().name = dir;
            directory = &directory->subdirs.last();
        }
    }

    for (Value& val : directory->values) {
        if (val.name == pathParts.last()) {
            if (!val.readonly) {
                val.function = function;
            }

            return;
        }
    }

    directory->values.add(pathParts.last(), function, readonly);
}

void Sysfs::remove(const String& path) {
    auto pathParts = path.split('/');
    if (pathParts.size() == 0)
        return;

    Directory* directory = &root;
    for (size_t i = 0; i < pathParts.size() - 1; ++i) {
        auto dir = pathParts[i];

        bool found = false;
        for (Directory& subdir : directory->subdirs) {
            if (subdir.name == dir) {
                directory = &subdir;
                found = true;
                continue;
            }
        }

        if (!found)
            return;
    }

    for (Value& val : directory->values) {
        if (val.name == pathParts.last()) {
            val.name = "";
        }
    }
}

String Sysfs::fileList() {
    auto wd = Scheduler::getWorkingDirectory();

    Directory* directory = &root;
    for (size_t i = 0; i < wd.size(); ++i) {
        auto dir = wd[i];

        bool found = false;
        for (Directory& subdir : directory->subdirs) {
            if (subdir.name == dir) {
                directory = &subdir;
                found = true;
                break;
            }
        }

        if (!found) {
            return "";
        }
    }

    String files;
    for (Value& val : directory->values) {
        files += val.name + " ";
    }

    return files;
}

String Sysfs::directoryList() {
    auto wd = Scheduler::getWorkingDirectory();

    Directory* directory = &root;
    for (size_t i = 0; i < wd.size(); ++i) {
        auto dir = wd[i];

        bool found = false;
        for (Directory& subdir : directory->subdirs) {
            if (subdir.name == dir) {
                directory = &subdir;
                found = true;
                break;
            }
        }

        if (!found) {
            return "";
        }
    }

    String dirs;
    for (Directory& dir : directory->subdirs) {
        dirs += dir.name + " ";
    }

    return dirs;
}
