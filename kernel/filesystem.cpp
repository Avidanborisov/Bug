#include "filesystem.hpp"
#include "string.hpp"
#include "scheduler.hpp"
#include "sysfs.hpp"

Vector<Filesystem::Backend*> Filesystem::mounted;

void Filesystem::init() {
    mounted.add(new Sysfs("/"));

    Sysfs::set("/task/wd", [] {
        auto wd = Scheduler::getWorkingDirectory();
        String path = "/";

        for (auto& part : wd) {
            path += part + "/";
        }

        return path;
    });

    Sysfs::set("/task/files", Sysfs::fileList);
    Sysfs::set("/task/dirs", Sysfs::directoryList);
}

int Filesystem::read(const char* file, char* buffer, size_t count, size_t offset) {
    auto path = getPath(file);
    auto& fs = getBackend(path);
    auto fullPath = getBackendPath(path, fs);

    return fs.read(fullPath, buffer, count, offset);
}

int Filesystem::write(const char* file, const char* buffer, size_t count, size_t offset) {
    auto path = getPath(file);
    auto& fs = getBackend(path);
    auto fullPath = getBackendPath(path, fs);

    return fs.write(fullPath, buffer, count, offset);
}

Vector<String> Filesystem::getBackendPath(const Vector<String>& fullPath, const Filesystem::Backend& fs) {
    Vector<String> path;

    for (size_t i = fs.mountPoint.size(); i < fullPath.size(); ++i) {
        path.add(fullPath[i]);
    }

    return path;
}

Vector<String> Filesystem::getPath(const char* filePath) {
    String file(filePath);
    Vector<String> path;

    if (file[0] != '/') {
        for (auto& part : Scheduler::getWorkingDirectory()) {
            path.add(part);
        }
    }

    auto parts = file.split('/');
    for (auto& part : parts) {
        path.add(part);
    }

    return path;
}

Filesystem::Backend& Filesystem::getBackend(const Vector<String>& path) {
    if (path.empty()) {
        for (auto& fs : mounted) {
            if (fs->mountPoint.empty()) {
                return *fs;
            }
        }
    }

    size_t bestSize = 0;
    size_t bestIndex = 0;

    for (size_t i = 0; i < mounted.size(); ++i) {
        auto& fs = mounted[i];

        bool match = true;
        for (size_t i = 0; i < fs->mountPoint.size() && i < path.size(); ++i) {
            if (path[i] != fs->mountPoint[i]) {
                match = false;
                break;
            }
        }

        if (match && fs->mountPoint.size() > bestSize) {
            bestSize = fs->mountPoint.size();
            bestIndex = i;
        }
    }

    return *mounted[bestIndex];
}

Filesystem::Backend::Backend(const String& mp) {
    mountPoint = mp.split('/');
}

int read(const char* file, char* buffer, size_t count, size_t offset) {
    return Filesystem::read(file, buffer, count, offset);
}

int write(const char* file, const char* buffer, size_t count, size_t offset) {
    return Filesystem::write(file, buffer, count, offset);
}
