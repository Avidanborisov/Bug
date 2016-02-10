#ifndef UNIQUEPOINTER_HPP
#define UNIQUEPOINTER_HPP

template<class T>
class DefaultDeleter {
public:
    constexpr DefaultDeleter() = default;
    constexpr DefaultDeleter(const DefaultDeleter&) = default;

    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "Can't delete incomplete type");
        delete ptr;
    }
};

template<class T>
class DefaultDeleter<T[]> {
    constexpr DefaultDeleter() = default;
    constexpr DefaultDeleter(const DefaultDeleter&) = default;

    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "Can't delete incomplete type");
        delete[] ptr;
    }
};

template<class T, class Deleter = DefaultDeleter<T> >
class UniquePointer {
private:
    T* ptr = nullptr;
    Deleter deleter { };

public:
    UniquePointer() = default;
    UniquePointer(decltype(nullptr)) : UniquePointer() { }

    UniquePointer(const UniquePointer&) = delete;
    UniquePointer& operator=(const UniquePointer&) = delete;

    explicit UniquePointer(T* ptr) : ptr(ptr), deleter(){ }

    ~UniquePointer() {
        reset();
    }

    UniquePointer& operator=(decltype(nullptr)) {
        reset();
        return *this;
    }

    UniquePointer(UniquePointer&& other) : ptr(other.release()), deleter(other.deleter) { }

    UniquePointer& operator=(UniquePointer&& other) {
        reset(other.release());
        deleter = other.deleter;
        return *this;
    }

    T* release() {
        T* p = ptr;
        ptr = nullptr;
        return p;
    }

    void reset(T* p = nullptr) {
        if (ptr != nullptr)
            deleter(ptr);

        ptr = p;
    }

    T* get() const {
        return ptr;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    explicit operator bool() const {
        return ptr == nullptr ? false : true;
    }
};

template<class T, class Deleter>
class UniquePointer<T[], Deleter> {
private:
    T* ptr = nullptr;
    Deleter deleter { };

public:
    UniquePointer() = default;
    UniquePointer(decltype(nullptr)) : UniquePointer() { }

    UniquePointer(const UniquePointer&) = delete;
    UniquePointer& operator=(const UniquePointer&) = delete;

    explicit UniquePointer(T* ptr) : ptr(ptr), deleter(){ }

    ~UniquePointer() {
        reset();
    }

    UniquePointer& operator=(decltype(nullptr)) {
        reset();
        return *this;
    }

    UniquePointer(UniquePointer&& other) : ptr(other.release()), deleter(other.deleter) { }

    UniquePointer& operator=(UniquePointer&& other) {
        reset(other.release());
        deleter = other.deleter;
        return *this;
    }

    T* release() {
        T* p = ptr;
        ptr = nullptr;
        return p;
    }

    void reset(T* p = nullptr) {
        if (ptr != nullptr)
            deleter(ptr);

        ptr = p;
    }

    T* get() const {
        return ptr;
    }

    T& operator[](size_t i) const {
        return ptr[i];
    }

    explicit operator bool() const {
        return ptr == nullptr ? false : true;
    }
};

static_assert(sizeof(UniquePointer<int>) == sizeof(int*), "UniquePointer with DefaultDeleter has no overhead");

#endif // UNIQUEPOINTER_HPP
