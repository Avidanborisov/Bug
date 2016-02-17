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

/*
 * UniquePointer members are stored in this seperate class in order to employ
 * empty base class optimization for the deleter object (so that empty deleters
 * have no memory overhead).
 */
template<class T, class Deleter, bool = __is_empty(Deleter) && !__is_final(Deleter)>
class UniquePointerData : private Deleter {
public:
    UniquePointerData(T* ptr, Deleter deleter) : Deleter(deleter), pointer(ptr) { }

    T*& ptr()                      { return pointer; }
    const T*& ptr() const          { return pointer; }

    Deleter& deleter()             { return *this; }
    const Deleter& deleter() const { return *this; }

private:
    T* pointer;
};

template<class T, class Deleter>
class UniquePointerData<T, Deleter, false> {
public:
    UniquePointerData(T* ptr, Deleter deleter) : pointer(ptr), del(deleter) { }

    T*& ptr()                      { return pointer; }
    const T*& ptr() const          { return pointer; }

    Deleter& deleter()             { return del; }
    const Deleter& deleter() const { return del; }

private:
    T* pointer;
    Deleter del;
};

template<class T, class Deleter = DefaultDeleter<T>>
class UniquePointer {
public:
    UniquePointer() = default;
    UniquePointer(decltype(nullptr)) : UniquePointer() { }

    UniquePointer(const UniquePointer&) = delete;
    UniquePointer& operator=(const UniquePointer&) = delete;

    explicit UniquePointer(T* ptr) : data(ptr, Deleter()) { }

    ~UniquePointer() {
        reset();
    }

    UniquePointer& operator=(decltype(nullptr)) {
        reset();
        return *this;
    }

    UniquePointer(UniquePointer&& other) : data(other.release(), other.data.deleter()) { }

    UniquePointer& operator=(UniquePointer&& other) {
        reset(other.release());
        data.deleter() = other.data.deleter();
        return *this;
    }

    T* release() {
        T* ptr = data.ptr();
        data.ptr() = nullptr;
        return ptr;
    }

    void reset(T* ptr = nullptr) {
        if (data.ptr() != nullptr)
            data.deleter()(data.ptr());

        data.ptr() = ptr;
    }

    T* get() const {
        return data.ptr();
    }

    T& operator*() const {
        return *data.ptr();
    }

    T* operator->() const {
        return data.ptr();
    }

    explicit operator bool() const {
        return data.ptr() == nullptr ? false : true;
    }

private:
    UniquePointerData<T, Deleter> data { nullptr, { } };
};

template<class T, class Deleter>
class UniquePointer<T[], Deleter> {
public:
    UniquePointer() = default;
    UniquePointer(decltype(nullptr)) : UniquePointer() { }

    UniquePointer(const UniquePointer&) = delete;
    UniquePointer& operator=(const UniquePointer&) = delete;

    explicit UniquePointer(T* ptr) : data(ptr, Deleter()) { }

    ~UniquePointer() {
        reset();
    }

    UniquePointer& operator=(decltype(nullptr)) {
        reset();
        return *this;
    }

    UniquePointer(UniquePointer&& other) : data(other.release(), other.data.deleter()) { }

    UniquePointer& operator=(UniquePointer&& other) {
        reset(other.release());
        data.deleter() = other.data.deleter();
        return *this;
    }

    T* release() {
        T* ptr = data.ptr();
        data.ptr() = nullptr;
        return ptr;
    }

    void reset(T* ptr = nullptr) {
        if (data.ptr() != nullptr)
            deleter(data.ptr());

        data.ptr() = ptr;
    }

    T* get() const {
        return data.ptr();
    }

    T& operator[](size_t i) const {
        return data.ptr()[i];
    }

    explicit operator bool() const {
        return data.ptr() == nullptr ? false : true;
    }

private:
    UniquePointerData<T, Deleter> data { nullptr, { } };
};

static_assert(sizeof(UniquePointer<int>)   == sizeof(int*), "UniquePointer with DefaultDeleter has no overhead");
static_assert(sizeof(UniquePointer<int[]>) == sizeof(int*), "UniquePointer with DefaultDeleter has no overhead");

#endif // UNIQUEPOINTER_HPP
