#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "vector.hpp"

template<class T>
class Queue {
public:
    bool empty() const {
        return vec.empty();
    }

    size_t size() const {
        return vec.size();
    }

    void push(const T& value) {
        vec.insert(0, { value });
    }

    T& top() {
        return vec.last();
    }

    const T& top() const {
        return vec.last();
    }

    void pop() {
        vec.pop();
    }

private:
    Vector<T> vec;
};

#endif // QUEUE_HPP
