#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "utility.hpp"

template<class T>
class Vector {
public:
    Vector() : data_(nullptr), size_(0), capacity_(0) { }

    template<class U>
    Vector(Util::Initializer<U> list) : Vector(list.begin(), list.end()) { }

    Vector(const Vector& other) : Vector(other.begin(), other.end()) { }

    template<class InputIt>
    Vector(InputIt first, InputIt last) {
        using Category = typename Util::IteratorTraits<InputIt>::Category;
        initialize(first, last, Category());
    }

    Vector& operator=(const Vector& other) {
        if (this != &other)
            assign(other.begin(), other.end());

        return *this;
    }

    template<class U>
    Vector& operator=(Util::Initializer<U> list) {
        assign(list.begin(), list.end());
        return *this;
    }

    Vector(Vector&& other) : Vector() {
        swap(other);
    }

    Vector& operator=(Vector&& other) {
        destruct();
        new (this) Vector();
        swap(other);
        return *this;
    }

    ~Vector() {
        destruct();
    }

    void swap(Vector& other) {
        Util::swap(data_, other.data_);
        Util::swap(size_, other.size_);
        Util::swap(capacity_, other.capacity_);
    }

    void clear() {
        destroy();
        size_ = 0;
    }

    template<class InputIt>
    void assign(InputIt first, InputIt last) {
        using Category = typename Util::IteratorTraits<InputIt>::Category;
        assign(first, last, Category());
    }

    template<class... Args>
    void resize(size_t newSize, Args&&... args) {
        if (newSize > size()) {
            reserve(newSize);
            Util::constructRange(end(), begin() + newSize, Util::forward<Args>(args)...);
        } else {
            Util::destroy(begin() + newSize, end());
        }

        size_ = newSize;
    }

    void reserve(size_t newCapacity) {
        if (newCapacity <= capacity())
            return;

        newCapacity = Util::max(newCapacity, capacity() * 2);
        auto newData = allocate(newCapacity);

        Util::constructMove(newData, begin(), end());
        destruct();

        data_ = newData;
        capacity_ = newCapacity;
    }

    template<class... Args>
    void add(Args&&... args) {
        reserve(size() + 1);
        Util::construct(end(), Util::forward<Args>(args)...);
        ++size_;
    }

    void push(const T& value) {
        add(value);
    }

    void push(T&& value) {
        add(Util::move(value));
    }

    template<class... Args>
    void emplace(size_t pos, Args&&... args) {
        if (size() != capacity()) {
            Util::construct(end(), Util::move(end()[-1]));
            Util::moveBackwards(end() - 1, begin() + pos, end() - 2);
            begin()[pos] = T(Util::forward<Args>(args)...);
        } else {
            const auto newCapacity = calcNewCapacity(size() + 1);
            auto newData = allocate(newCapacity);

            Util::constructMove(newData, begin(), begin() + pos);
            Util::construct(newData + pos, Util::forward<Args>(args)...);
            Util::constructMove(newData + pos + 1, begin() + pos, end());

            destruct();
            data_ = newData;
            capacity_ = newCapacity;
        }

        ++size_;
    }

    template<class InputIt>
    void insert(size_t pos, InputIt first, InputIt last) {
        using Category = typename Util::IteratorTraits<InputIt>::Category;
        insert(pos, first, last, Category());
    }

    template<class U>
    void insert(size_t pos, Util::Initializer<U> list) {
        insert(pos, list.begin(), list.end());
    }

    void pop() {
        --size_;
        Util::destroy(end());
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    bool empty() const {
        return size_ == 0;
    }

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    T& first() {
        return data_[0];
    }

    const T& first() const {
        return data_[0];
    }

    T& last() {
        return data_[size_ - 1];
    }

    const T& last() const {
        return data_[size_ - 1];
    }

    T* begin() {
        return data_;
    }

    const T* begin() const {
        return data_;
    }

    T* end() {
        return data_ + size_;
    }

    const T* end() const {
        return data_ + size_;
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;

    T* allocate(size_t n) {
        return static_cast<T*>(operator new(n * sizeof(T)));
    }

    void deallocate(void* p) {
        operator delete(p);
    }

    void destroy() {
        Util::destroy(begin(), end());
    }

    void destruct() {
        destroy();
        deallocate(data_);
    }

    size_t calcNewCapacity(size_t newCapacity) {
        return Util::max(newCapacity, capacity() * 2);
    }

    template<class InputIt>
    void initialize(InputIt first, InputIt last, Util::InputIteratorTag) {
        data_ = nullptr;
        size_ = capacity_ = 0;

        while (first != last) {
            add(*first);
            ++first;
        }
    }

    template<class ForwardIt>
    void initialize(ForwardIt first, ForwardIt last, Util::ForwardIteratorTag) {
        auto size = Util::distance(first, last);
        data_ = allocate(size);
        size_ = capacity_ = size;
        Util::constructCopy(begin(), first, last);
    }

    template<class InputIt>
    void assign(InputIt first, InputIt last, Util::InputIteratorTag) {
        clear();

        while (first != last) {
            add(*first);
            ++first;
        }
    }

    template<class ForwardIt>
    void assign(ForwardIt first, ForwardIt last, Util::ForwardIteratorTag) {
        const size_t newSize = Util::distance(first, last);

        if (newSize > capacity()) { // reallocate
            auto newData = allocate(newSize);
            Util::constructCopy(newData, first, last);

            destruct();
            data_ = newData;
            capacity_ = newSize;
        } else { // enough capacity
            if (newSize > size()) {
                auto allocatedLast = Util::next(first, size());
                Util::copy(begin(), first, allocatedLast);
                Util::constructCopy(end(), allocatedLast, last);
            } else {
                auto copiedLast = Util::copy(begin(), first, last);
                Util::destroy(copiedLast, end());
            }
        }

        size_ = newSize;
    }

    template<class InputIt>
    void insert(size_t pos, InputIt first, InputIt last, Util::InputIteratorTag) {
        for (size_t pos = 0; first != last; ++pos) {
            emplace(pos, *first);
            ++first;
        }
    }

    template<class ForwardIt>
    void insert(size_t pos, ForwardIt first, ForwardIt last, Util::ForwardIteratorTag) {
        const size_t newElements = Util::distance(first, last);
        const size_t remainingCapacity = capacity() - size();
        const size_t elementsAfter = size() - pos;

        if (newElements == 0)
            return;

        if (remainingCapacity >= newElements) { // enough capacity
            if (elementsAfter > newElements) {
                Util::constructMove(end(), end() - newElements, end());
                Util::moveBackwards(end(), begin() + pos, end() - newElements);
                Util::copy(begin() + pos, first, last);
            } else {
                auto lastWithinSize = Util::next(first, elementsAfter);
                auto lastConstructed = Util::constructCopy(end(), lastWithinSize, last);
                Util::constructMove(lastConstructed, begin() + pos, end());
                Util::copy(begin() + pos, first, lastWithinSize);
            }
        } else { // reallocate
            const auto newCapacity = calcNewCapacity(size() + newElements);
            auto newData = allocate(newCapacity);

            auto newEnd = Util::constructMove(newData, begin(), begin() + pos);
            newEnd = Util::constructCopy(newEnd, first, last);
            Util::constructMove(newEnd, begin() + pos, end());

            destruct();
            data_ = newData;
            capacity_ = newCapacity;
        }

        size_ += newElements;
    }
};

#endif // VECTOR_HPP
