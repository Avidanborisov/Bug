#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <stddef.h>
#include "assert.hpp"

template<class T, size_t N>
class Array {
public:
    T& operator[](size_t pos) {
        assert(pos < size_);
        return data_[pos];
    }

    const T& operator[](size_t pos) const {
        assert(pos < size_);
        return data_[pos];
    }

    void insert(const T& val) {
        assert(size_ < N);
        data_[size_++] = val;
    }

    void remove(size_t pos) {
        assert(size_ != 0 && pos < size_);

        for (size_t i = pos; i < size_ - 1; ++i)
            data_[i] = data_[i + 1];

        --size_;
    }

    void pop() {
        assert(size_ != 0);
        --size_;
    }

    void clear() {
        size_ = 0;
    }

    size_t find(const T& val) const {
        for (size_t i = 0; i < size_; ++i)
            if (data_[i] == val)
                return i;

        return size_;
    }

    size_t capacity() const {
        return N;
    }

    size_t size() const {
        return size_;
    }

    T* data() {
        return data_;
    }

    const T* data() const {
        return data_;
    }

    T& first() {
        assert(size_ != 0);
        return data_[0];
    }

    const T& first() const {
        assert(size_ != 0);
        return data_[0];
    }

    T& last() {
        assert(size_ != 0);
        return data_[size_ - 1];
    }

    const T& last() const {
        assert(size_ != 0);
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
    T data_[N];
    size_t size_ = 0;
};

#endif // ARRAY_HPP
