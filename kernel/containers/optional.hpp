#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

template<class T>
class Optional {
public:
    constexpr Optional() : storage(), initialized(false) { }
    constexpr Optional(const T& value) : storage(value), initialized(true) { }

    Optional(const Optional& other) : initialized(other.initialized) {
        if (other.initialized)
            storage.value = other.storage.value;
    }

    ~Optional() {
        if (initialized)
            storage.value.~T();
    }

    constexpr explicit operator bool() const {
        return initialized;
    }

    constexpr const T& operator*() const {
        return storage.value;
    }

    constexpr const T* operator->() const {
        return &storage.value;
    }

    T& operator*() {
        return storage.value;
    }

    T* operator->() {
        return &storage.value;
    }

private:
    union Storage {
        T value;
        struct { } dummy;

        constexpr Storage() : dummy() { }
        constexpr Storage(const T& value) :  value(value) { }
        ~Storage() { }

        Storage& operator=(const Storage& other) {
            value = other.value;
            return *this;
        }
    } storage;

    bool initialized;
};

#endif // OPTIONAL_HPP
